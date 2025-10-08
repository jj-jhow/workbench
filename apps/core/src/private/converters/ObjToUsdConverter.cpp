#include "converters/ObjToUsdConverter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/types.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/array.h>
#include <pxr/usd/sdf/attributeSpec.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/namespaceEditor.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include <iostream>
#include <cmath>

namespace converters
{

    bool ObjToUsdConverter::Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const
    {
        std::cout << "Converting OBJ to USD: " << inputPath << " -> " << outputPath << std::endl;

        try
        {
            // Extract and transform
            pxr::UsdStageRefPtr stage = Extract(inputPath, outputPath);
            if (!stage)
            {
                std::cerr << "Failed to create USD stage: " << outputPath << std::endl;
                return false;
            }

            Transform(stage, options);

            // Save the stage
            stage->Save();

            std::cout << "Successfully created USD stage " << std::endl;
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error during conversion: " << e.what() << std::endl;
            return false;
        }
    }

    pxr::UsdStageRefPtr ObjToUsdConverter::Extract(const fs::path &inputPath, const fs::path &outputPath) const
    {
        std::cout << "Extracting data from: " << inputPath << " to " << outputPath << std::endl;

        // Using Assimp to read the OBJ file and extract the scene data before converting to USD
        const unsigned int importFlags = 0;
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(inputPath.string(), importFlags); // NOTE: Do NOT delete scene - Assimp::Importer manages memory automatically
        if (!scene)
        {
            std::cerr << "Failed to read data from OBJ file: " << inputPath << std::endl;
            return nullptr;
        }

        // Create a new USD stage so we can populate it with the extracted data
        auto stage = pxr::UsdStage::CreateNew(outputPath);
        if (!stage)
        {
            std::cerr << "Failed to create USD stage: " << outputPath << std::endl;
            return nullptr;
        }

        if (!scene->HasMeshes())
        {
            std::cerr << "Warning: No meshes found in OBJ file: " << inputPath << std::endl;
            return stage;
        }

        // The actual data conversion from aiScene to USD
        aiMesh *const *meshes = scene->mMeshes;
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh const *mesh = meshes[i];
            if (!mesh)
            {
                std::cerr << "Invalid mesh found in scene." << std::endl;
                continue;
            }

            // Convert each mesh to a USD mesh
            ExtractMeshData(mesh, scene, stage);
        }

        return stage;
    }

    void ObjToUsdConverter::Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const
    {
        if (!stage)
        {
            std::cerr << "Invalid USD stage." << std::endl;
            return;
        }

        SetDefaultPrim(stage);
        SetUpAxis(stage, options.upAxis);
        SetMetersPerUnit(stage, options.linearUnit);
    }

    void ObjToUsdConverter::ExtractMeshData(const aiMesh *mesh, const aiScene *scene, pxr::UsdStageRefPtr stage) const
    {
        if (!mesh || !stage || !scene)
        {
            std::cerr << "Invalid mesh or stage for conversion." << std::endl;
            return;
        }

        std::string meshName = pxr::TfMakeValidIdentifier(mesh->mName.C_Str());

        // Create a USD mesh at the root level with a unique name
        pxr::UsdGeomMesh usdMesh = pxr::UsdGeomMesh::Define(stage, pxr::SdfPath("/" + meshName));

        // Convert vertex to points
        pxr::VtArray<pxr::GfVec3f> points;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            const ::aiVector3D &v = mesh->mVertices[i];
            points.push_back(pxr::GfVec3f(v.x, v.y, v.z));
        }

        usdMesh.CreatePointsAttr().Set(points);

        // Convert faces to faceVertexIndices and faceVertexCounts
        pxr::VtArray<int> faceVertexIndices;
        pxr::VtArray<int> faceVertexCounts;

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const ::aiFace &face = mesh->mFaces[i];
            faceVertexCounts.push_back(face.mNumIndices);
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                faceVertexIndices.push_back(face.mIndices[j]);
            }
        }

        usdMesh.CreateFaceVertexCountsAttr().Set(faceVertexCounts);
        usdMesh.CreateFaceVertexIndicesAttr().Set(faceVertexIndices);

        // Convert texture coordinates to primvars:st

        // Note: OBJ format does not support normal maps.
        usdMesh.CreateSubdivisionSchemeAttr().Set(pxr::UsdGeomTokens->none);

        // Convert vertex normals to primvars:normals
        if (mesh->mNormals)
        {
            pxr::VtArray<pxr::GfVec3f> normals;
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                const ::aiVector3D &n = mesh->mNormals[i];
                normals.push_back(pxr::GfVec3f(n.x, n.y, n.z));
            }

            usdMesh.CreateNormalsAttr().Set(normals);
        }

        // Extract mesh's material data
        if (scene->HasMaterials() && mesh->mMaterialIndex < scene->mNumMaterials)
        {
            const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            if (material)
            {
                std::cout << "Extracting material for mesh: " << meshName << std::endl;
                // Extract material properties as needed
                pxr::UsdShadeMaterial usdMaterial = ExtractMaterialData(material, stage);
                if (usdMaterial)
                {
                    pxr::UsdShadeMaterialBindingAPI bindingAPI = pxr::UsdShadeMaterialBindingAPI::Apply(usdMesh.GetPrim());
                    bindingAPI.Bind(usdMaterial);
                    std::cout << "Bound material to mesh: " << meshName << std::endl;
                }
                else
                {
                    std::cerr << "Failed to extract material for mesh: " << meshName << std::endl;
                }
            }
        }

        std::cout << "Converted mesh: " << meshName << " with " << mesh->mNumVertices << " vertices and " << mesh->mNumFaces << " faces." << std::endl;
    }

    pxr::UsdShadeMaterial ObjToUsdConverter::ExtractMaterialData(const aiMaterial *material, pxr::UsdStageRefPtr stage) const
    {
        if (!material || !stage)
        {
            std::cerr << "Invalid material or stage for conversion." << std::endl;
            return pxr::UsdShadeMaterial();
        }

        aiString matName;
        if (material->Get(AI_MATKEY_NAME, matName) != AI_SUCCESS)
        {
            std::cerr << "Failed to get material name." << std::endl;
            return pxr::UsdShadeMaterial();
        }

        std::string materialName = pxr::TfMakeValidIdentifier(matName.C_Str());
        pxr::SdfPath materialPath = pxr::SdfPath("/Materials/" + materialName);

        pxr::UsdShadeMaterial usdMaterial = pxr::UsdShadeMaterial::Define(stage, materialPath);
        pxr::UsdShadeShader shader = pxr::UsdShadeShader::Define(stage, materialPath.AppendChild(pxr::TfToken("shader")));

        shader.CreateIdAttr(pxr::VtValue(pxr::TfToken("UsdPreviewSurface")));
        usdMaterial.CreateSurfaceOutput().ConnectToSource(shader.ConnectableAPI(), pxr::TfToken("surface"));

        shader.CreateInput(pxr::TfToken("useSpecularWorkflow"), pxr::SdfValueTypeNames->Int).Set(pxr::VtValue(1));

        // Extract diffuse color
        aiColor3D diffuseColor;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
        {
            shader.CreateInput(pxr::TfToken("diffuseColor"), pxr::SdfValueTypeNames->Color3f).Set(pxr::VtValue(pxr::GfVec3f(diffuseColor.r, diffuseColor.g, diffuseColor.b)));
        }
        // Extract Emissive color
        aiColor3D emissiveColor;
        if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == AI_SUCCESS)
        {
            shader.CreateInput(pxr::TfToken("emissiveColor"), pxr::SdfValueTypeNames->Color3f).Set(pxr::VtValue(pxr::GfVec3f(emissiveColor.r, emissiveColor.g, emissiveColor.b)));
        }
        // Extract specular color
        aiColor3D specularColor;
        if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
        {
            shader.CreateInput(pxr::TfToken("specularColor"), pxr::SdfValueTypeNames->Color3f).Set(pxr::VtValue(pxr::GfVec3f(specularColor.r, specularColor.g, specularColor.b)));
        }
        // Extract shininess
        float shininess = 0.0f;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
        {
            const float roughness = 1.0f - std::sqrt(shininess / 1000.0f);
            shader.CreateInput(pxr::TfToken("roughness"), pxr::SdfValueTypeNames->Float).Set(pxr::VtValue(roughness)); // Roughness is inverse of shininess
        }

        return usdMaterial;
    }

    bool ObjToUsdConverter::SetDefaultPrim(pxr::UsdStageRefPtr stage) const
    {
        bool result = true;
        if (!stage)
        {
            std::cerr << "Invalid USD stage." << std::endl;
            return false;
        }

        const pxr::UsdPrimSiblingRange rootPrims = stage->GetPseudoRoot().GetChildren();
        if (rootPrims.empty())
        {
            std::cerr << "No root prims found to set as default." << std::endl;
            return false;
        }

        std::cout << "Setting default prim to world." << std::endl;
        const pxr::UsdPrim worldPrim = pxr::UsdGeomXform::Define(stage, pxr::SdfPath("/World")).GetPrim();
        stage->SetDefaultPrim(worldPrim);

        pxr::UsdNamespaceEditor editor = pxr::UsdNamespaceEditor(stage);

        for (const pxr::UsdPrim &prim : rootPrims)
        {
            if (prim == worldPrim)
                continue;

            std::cout << "Reparenting prim: " << prim.GetName().GetString() << " under " << worldPrim.GetName().GetString() << std::endl;
            result = result && editor.ReparentPrim(prim, worldPrim) && editor.ApplyEdits();
            if (!result)
            {
                std::cerr << "Failed to reparent prim: " << prim.GetName().GetString() << std::endl;
                break;
            }
        }

        return result;
    }

    bool ObjToUsdConverter::SetUpAxis(pxr::UsdStageRefPtr stage, UpAxis upAxis) const
    {
        if (!stage)
        {
            std::cerr << "Invalid USD stage." << std::endl;
            return false;
        }

        const pxr::TfToken upAxisToken = UpAxisParser::toToken(upAxis);
        pxr::UsdGeomSetStageUpAxis(stage, upAxisToken);

        if (upAxis == UpAxis::Z)
        {
            return pxr::UsdGeomXformable(stage->GetDefaultPrim()).AddRotateXOp(pxr::UsdGeomXformOp::PrecisionFloat).Set(90.0f);
        }

        return true;
    }

    bool ObjToUsdConverter::SetMetersPerUnit(pxr::UsdStageRefPtr stage, LinearUnit linearUnit) const
    {
        if (!stage)
        {
            std::cerr << "Invalid USD stage." << std::endl;
            return false;
        }

        // Set linear units to meters
        const double unitInMeters = LinearUnitParser::toDouble(linearUnit);
        return pxr::UsdGeomSetStageMetersPerUnit(stage, unitInMeters);
    }

} // namespace converters
