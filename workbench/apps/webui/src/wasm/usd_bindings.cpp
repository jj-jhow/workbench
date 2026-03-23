#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/cone.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdShade/material.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"
#include "pxr/usd/usdShade/shader.h"
#include "pxr/usd/sdf/layer.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdUtils/stageCache.h"
#include "pxr/usd/ar/resolver.h"
#include "pxr/base/tf/token.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/vt/array.h"

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <algorithm>

PXR_NAMESPACE_USING_DIRECTIVE

using namespace emscripten;

// ── In-memory stage management ──────────────────────────────────────────────

static UsdStageRefPtr g_stage;
static int g_nextPrimId = 1;
static std::unordered_map<std::string, SdfPath> g_primIdToPath;

static void ensureStage() {
    if (!g_stage) {
        g_stage = UsdStage::CreateInMemory("scene.usda");
        UsdGeomXform::Define(g_stage, SdfPath("/World"));
        g_stage->SetDefaultPrim(g_stage->GetPrimAtPath(SdfPath("/World")));
    }
}

// ── Helpers: extract scene data from a stage ────────────────────────────────

static val extractTransform(const UsdPrim &prim) {
    val transform = val::object();
    UsdGeomXformable xformable(prim);
    if (xformable) {
        GfMatrix4d localXform;
        bool resetsXformStack;
        xformable.GetLocalTransformation(&localXform, &resetsXformStack);

        val matrix = val::array();
        const double *d = localXform.data();
        for (int i = 0; i < 16; ++i) {
            matrix.call<void>("push", d[i]);
        }
        transform.set("matrix", matrix);

        // Decompose into translate / rotate / scale for convenience
        GfVec3d translate = localXform.ExtractTranslation();
        val t = val::array();
        t.call<void>("push", translate[0]);
        t.call<void>("push", translate[1]);
        t.call<void>("push", translate[2]);
        transform.set("translate", t);
    }
    return transform;
}

static val extractMaterialInfo(const UsdPrim &prim) {
    val matInfo = val::object();
    UsdShadeMaterialBindingAPI bindingAPI(prim);
    if (bindingAPI) {
        UsdShadeMaterial mat = bindingAPI.ComputeBoundMaterial();
        if (mat) {
            matInfo.set("name", mat.GetPrim().GetName().GetString());
            matInfo.set("path", mat.GetPrim().GetPath().GetString());

            // Try to get surface shader colour
            UsdShadeShader surface = mat.ComputeSurfaceSource();
            if (surface) {
                UsdShadeInput diffuseInput = surface.GetInput(TfToken("diffuseColor"));
                if (diffuseInput) {
                    GfVec3f color;
                    if (diffuseInput.Get(&color)) {
                        val c = val::array();
                        c.call<void>("push", color[0]);
                        c.call<void>("push", color[1]);
                        c.call<void>("push", color[2]);
                        matInfo.set("diffuseColor", c);
                    }
                }
            }
        }
    }
    return matInfo;
}

static val traverseStage(const UsdStageRefPtr &stage) {
    val result = val::object();
    val primitives = val::array();
    val cameras = val::array();
    val lights = val::array();
    val materials = val::array();

    std::unordered_map<std::string, bool> seenMaterials;

    for (UsdPrim prim : stage->Traverse()) {
        std::string typeName = prim.GetTypeName().GetString();
        std::string path = prim.GetPath().GetString();
        std::string name = prim.GetName().GetString();

        // Collect materials at the end
        if (prim.IsA<UsdShadeMaterial>()) {
            if (seenMaterials.find(path) == seenMaterials.end()) {
                seenMaterials[path] = true;
                val m = val::object();
                m.set("name", name);
                m.set("path", path);

                UsdShadeMaterial mat(prim);
                UsdShadeShader surface = mat.ComputeSurfaceSource();
                if (surface) {
                    UsdShadeInput diffuseInput = surface.GetInput(TfToken("diffuseColor"));
                    if (diffuseInput) {
                        GfVec3f color;
                        if (diffuseInput.Get(&color)) {
                            val c = val::array();
                            c.call<void>("push", color[0]);
                            c.call<void>("push", color[1]);
                            c.call<void>("push", color[2]);
                            m.set("diffuseColor", c);
                        }
                    }
                }
                materials.call<void>("push", m);
            }
            continue;
        }

        // Cameras
        if (typeName == "Camera") {
            val cam = val::object();
            cam.set("name", name);
            cam.set("path", path);
            cam.set("type", typeName);
            cam.set("transform", extractTransform(prim));
            cameras.call<void>("push", cam);
            continue;
        }

        // Lights
        if (typeName.find("Light") != std::string::npos) {
            val light = val::object();
            light.set("name", name);
            light.set("path", path);
            light.set("type", typeName);
            light.set("transform", extractTransform(prim));
            lights.call<void>("push", light);
            continue;
        }

        // Geometry primitives (Mesh, Cube, Sphere, Cone, Cylinder, Xform…)
        if (prim.IsA<UsdGeomGprim>() || prim.IsA<UsdGeomXform>()) {
            val p = val::object();
            p.set("name", name);
            p.set("path", path);
            p.set("type", typeName);
            p.set("transform", extractTransform(prim));
            p.set("material", extractMaterialInfo(prim));
            primitives.call<void>("push", p);
        }
    }

    result.set("primitives", primitives);
    result.set("cameras", cameras);
    result.set("lights", lights);
    result.set("materials", materials);
    return result;
}

// ── Public API ──────────────────────────────────────────────────────────────

val unpackUSDZ(const std::string &data) {
    // Write the raw bytes to a temp file so USD can open it
    const std::string tmpPath = "/tmp/_input.usdz";
    {
        std::ofstream ofs(tmpPath, std::ios::binary);
        ofs.write(data.data(), data.size());
    }

    UsdStageRefPtr stage = UsdStage::Open(tmpPath);
    if (!stage) {
        val err = val::object();
        err.set("error", std::string("Failed to open USDZ stage"));
        return err;
    }

    return traverseStage(stage);
}

val packUSDZ(const std::string &jsonDesc) {
    // Create a temporary stage from a JSON-ish description, export to USDZ,
    // and return the bytes.  For now the input is treated as a USDA string
    // that the caller can compose on the JS side.
    const std::string tmpUsda = "/tmp/_pack_input.usda";
    const std::string tmpUsdz = "/tmp/_pack_output.usdz";
    {
        std::ofstream ofs(tmpUsda, std::ios::binary);
        ofs.write(jsonDesc.data(), jsonDesc.size());
    }

    UsdStageRefPtr stage = UsdStage::Open(tmpUsda);
    if (!stage) {
        return val::null();
    }

    stage->Export(tmpUsdz);

    std::ifstream ifs(tmpUsdz, std::ios::binary | std::ios::ate);
    if (!ifs) {
        return val::null();
    }
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::string buf(size, '\0');
    ifs.read(&buf[0], size);

    return val(buf);
}

val createPrimitive(const std::string &type, val properties) {
    ensureStage();

    std::string id = std::to_string(g_nextPrimId++);
    std::string primName = "Prim_" + id;
    SdfPath primPath = SdfPath("/World/" + primName);

    // Determine prim type and define it
    if (type == "Cube") {
        UsdGeomCube::Define(g_stage, primPath);
    } else if (type == "Sphere") {
        UsdGeomSphere::Define(g_stage, primPath);
    } else if (type == "Cone") {
        UsdGeomCone::Define(g_stage, primPath);
    } else if (type == "Cylinder") {
        UsdGeomCylinder::Define(g_stage, primPath);
    } else if (type == "Mesh") {
        UsdGeomMesh::Define(g_stage, primPath);
    } else {
        // Default to Xform for unknown types
        UsdGeomXform::Define(g_stage, primPath);
    }

    UsdPrim prim = g_stage->GetPrimAtPath(primPath);
    if (!prim.IsValid()) {
        return val::null();
    }

    g_primIdToPath[id] = primPath;

    // Apply transform properties if provided
    UsdGeomXformable xformable(prim);
    if (xformable) {
        if (properties.hasOwnProperty("translate")) {
            val t = properties["translate"];
            GfVec3d translate(
                t[0].as<double>(),
                t[1].as<double>(),
                t[2].as<double>()
            );
            UsdGeomXformOp translateOp = xformable.AddTranslateOp();
            translateOp.Set(translate);
        }
        if (properties.hasOwnProperty("rotate")) {
            val r = properties["rotate"];
            GfVec3f rotate(
                r[0].as<float>(),
                r[1].as<float>(),
                r[2].as<float>()
            );
            UsdGeomXformOp rotateOp = xformable.AddRotateXYZOp();
            rotateOp.Set(rotate);
        }
        if (properties.hasOwnProperty("scale")) {
            val s = properties["scale"];
            GfVec3f scale(
                s[0].as<float>(),
                s[1].as<float>(),
                s[2].as<float>()
            );
            UsdGeomXformOp scaleOp = xformable.AddScaleOp();
            scaleOp.Set(scale);
        }
    }

    // Apply material colour if provided
    if (properties.hasOwnProperty("color")) {
        val c = properties["color"];
        GfVec3f color(
            c[0].as<float>(),
            c[1].as<float>(),
            c[2].as<float>()
        );

        SdfPath matPath = SdfPath("/World/Materials/Mat_" + id);
        UsdShadeMaterial mat = UsdShadeMaterial::Define(g_stage, matPath);

        SdfPath shaderPath = matPath.AppendChild(TfToken("PBRShader"));
        UsdShadeShader shader = UsdShadeShader::Define(g_stage, shaderPath);
        shader.CreateIdAttr(VtValue(TfToken("UsdPreviewSurface")));
        shader.CreateInput(TfToken("diffuseColor"), SdfValueTypeNames->Color3f).Set(color);

        UsdShadeOutput surfaceOutput = shader.CreateOutput(TfToken("surface"), SdfValueTypeNames->Token);
        mat.CreateSurfaceOutput().ConnectToSource(surfaceOutput);

        UsdShadeMaterialBindingAPI(prim).Bind(mat);
    }

    val result = val::object();
    result.set("id", id);
    result.set("path", primPath.GetString());
    result.set("type", type);
    return result;
}

void updatePrimitive(const std::string &id, val properties) {
    if (!g_stage) return;

    auto it = g_primIdToPath.find(id);
    if (it == g_primIdToPath.end()) return;

    UsdPrim prim = g_stage->GetPrimAtPath(it->second);
    if (!prim.IsValid()) return;

    UsdGeomXformable xformable(prim);
    if (!xformable) return;

    // Clear existing xform ops and reapply
    xformable.ClearXformOpOrder();

    if (properties.hasOwnProperty("translate")) {
        val t = properties["translate"];
        GfVec3d translate(
            t[0].as<double>(),
            t[1].as<double>(),
            t[2].as<double>()
        );
        UsdGeomXformOp translateOp = xformable.AddTranslateOp();
        translateOp.Set(translate);
    }
    if (properties.hasOwnProperty("rotate")) {
        val r = properties["rotate"];
        GfVec3f rotate(
            r[0].as<float>(),
            r[1].as<float>(),
            r[2].as<float>()
        );
        UsdGeomXformOp rotateOp = xformable.AddRotateXYZOp();
        rotateOp.Set(rotate);
    }
    if (properties.hasOwnProperty("scale")) {
        val s = properties["scale"];
        GfVec3f scale(
            s[0].as<float>(),
            s[1].as<float>(),
            s[2].as<float>()
        );
        UsdGeomXformOp scaleOp = xformable.AddScaleOp();
        scaleOp.Set(scale);
    }

    // Update material colour if needed
    if (properties.hasOwnProperty("color")) {
        val c = properties["color"];
        GfVec3f color(
            c[0].as<float>(),
            c[1].as<float>(),
            c[2].as<float>()
        );

        UsdShadeMaterialBindingAPI bindingAPI(prim);
        UsdShadeMaterial mat = bindingAPI.ComputeBoundMaterial();
        if (mat) {
            UsdShadeShader surface = mat.ComputeSurfaceSource();
            if (surface) {
                UsdShadeInput diffuseInput = surface.GetInput(TfToken("diffuseColor"));
                if (diffuseInput) {
                    diffuseInput.Set(color);
                }
            }
        }
    }
}

void deletePrimitive(const std::string &id) {
    if (!g_stage) return;

    auto it = g_primIdToPath.find(id);
    if (it == g_primIdToPath.end()) return;

    g_stage->RemovePrim(it->second);
    g_primIdToPath.erase(it);
}

val exportUSDZ() {
    ensureStage();

    const std::string tmpPath = "/tmp/_export.usdz";
    g_stage->Export(tmpPath);

    std::ifstream ifs(tmpPath, std::ios::binary | std::ios::ate);
    if (!ifs) {
        return val::null();
    }
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::string buf(size, '\0');
    ifs.read(&buf[0], size);

    return val(buf);
}

// ── Embind registration ─────────────────────────────────────────────────────

EMSCRIPTEN_BINDINGS(openusd_module) {
    function("unpackUSDZ", &unpackUSDZ);
    function("packUSDZ", &packUSDZ);
    function("createPrimitive", &createPrimitive);
    function("updatePrimitive", &updatePrimitive);
    function("deletePrimitive", &deletePrimitive);
    function("exportUSDZ", &exportUSDZ);
}
