#include "MaterialAuthor.h"
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/tokens.h>
#include <pxr/usd/usd/primRange.h>
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace author
    {
        UsdShadeMaterial MaterialAuthor::createMaterial(UsdStagePtr stage, const std::string &materialPath)
        {
            if (!stage)
            {
                std::cerr << "Invalid stage provided" << std::endl;
                return UsdShadeMaterial();
            }

            SdfPath path(materialPath);
            if (!path.IsAbsolutePath())
            {
                std::cerr << "Material path must be absolute: " << materialPath << std::endl;
                return UsdShadeMaterial();
            }

            return UsdShadeMaterial::Define(stage, path);
        }

        UsdShadeMaterial MaterialAuthor::createBasicMaterial(UsdStagePtr stage, const std::string &materialPath, const GfVec3f &diffuseColor)
        {
            UsdShadeMaterial material = createMaterial(stage, materialPath);
            if (!material)
            {
                return UsdShadeMaterial();
            }

            // Create a UsdPreviewSurface shader
            UsdShadeShader shader = createPreviewSurfaceShader(material);
            if (shader)
            {
                setDiffuseColor(shader, diffuseColor);
                connectShaderToMaterial(shader, material);
            }

            return material;
        }

        UsdShadeMaterial MaterialAuthor::createPBRMaterial(UsdStagePtr stage, const std::string &materialPath)
        {
            UsdShadeMaterial material = createMaterial(stage, materialPath);
            if (!material)
            {
                return UsdShadeMaterial();
            }

            // Create a UsdPreviewSurface shader with PBR defaults
            UsdShadeShader shader = createPreviewSurfaceShader(material);
            if (shader)
            {
                // Set PBR defaults
                setDiffuseColor(shader, GfVec3f(0.8f, 0.8f, 0.8f));
                setMetallic(shader, 0.0f);
                setRoughness(shader, 0.5f);
                connectShaderToMaterial(shader, material);
            }

            return material;
        }

        UsdShadeShader MaterialAuthor::createPreviewSurfaceShader(UsdShadeMaterial &material, const std::string &shaderName)
        {
            if (!material)
            {
                return UsdShadeShader();
            }

            SdfPath shaderPath = material.GetPath().AppendChild(TfToken(shaderName));
            UsdShadeShader shader = UsdShadeShader::Define(material.GetPrim().GetStage(), shaderPath);

            if (shader)
            {
                shader.CreateIdAttr(VtValue(TfToken("UsdPreviewSurface")));
            }

            return shader;
        }

        UsdShadeShader MaterialAuthor::createTextureShader(UsdShadeMaterial &material, const std::string &texturePath, const std::string &shaderName)
        {
            if (!material)
            {
                return UsdShadeShader();
            }

            SdfPath shaderPath = material.GetPath().AppendChild(TfToken(shaderName));
            UsdShadeShader shader = UsdShadeShader::Define(material.GetPrim().GetStage(), shaderPath);

            if (shader)
            {
                shader.CreateIdAttr(VtValue(TfToken("UsdUVTexture")));
                shader.CreateInput(TfToken("file"), SdfValueTypeNames->Asset).Set(SdfAssetPath(texturePath));
            }

            return shader;
        }

        void MaterialAuthor::setDiffuseColor(UsdShadeShader &shader, const GfVec3f &color)
        {
            if (shader)
            {
                shader.CreateInput(TfToken("diffuseColor"), SdfValueTypeNames->Color3f).Set(color);
            }
        }

        void MaterialAuthor::setMetallic(UsdShadeShader &shader, float metallic)
        {
            if (shader)
            {
                shader.CreateInput(TfToken("metallic"), SdfValueTypeNames->Float).Set(metallic);
            }
        }

        void MaterialAuthor::setRoughness(UsdShadeShader &shader, float roughness)
        {
            if (shader)
            {
                shader.CreateInput(TfToken("roughness"), SdfValueTypeNames->Float).Set(roughness);
            }
        }

        void MaterialAuthor::setOpacity(UsdShadeShader &shader, float opacity)
        {
            if (shader)
            {
                shader.CreateInput(TfToken("opacity"), SdfValueTypeNames->Float).Set(opacity);
            }
        }

        void MaterialAuthor::setEmissiveColor(UsdShadeShader &shader, const GfVec3f &color)
        {
            if (shader)
            {
                shader.CreateInput(TfToken("emissiveColor"), SdfValueTypeNames->Color3f).Set(color);
            }
        }

        bool MaterialAuthor::assignMaterial(UsdPrim &prim, UsdShadeMaterial &material)
        {
            if (!prim || !material)
            {
                return false;
            }

            UsdShadeMaterialBindingAPI bindingAPI(prim);
            return bindingAPI.Bind(material);
        }

        bool MaterialAuthor::clearMaterialAssignment(UsdPrim &prim)
        {
            if (!prim)
            {
                return false;
            }

            UsdShadeMaterialBindingAPI bindingAPI(prim);
            return bindingAPI.UnbindAllBindings();
        }

        UsdShadeMaterial MaterialAuthor::getMaterialFromPrim(UsdPrim &prim)
        {
            if (!prim)
            {
                return UsdShadeMaterial();
            }

            UsdShadeMaterialBindingAPI bindingAPI(prim);
            return bindingAPI.ComputeBoundMaterial();
        }

        std::vector<UsdShadeMaterial> MaterialAuthor::getAllMaterials(UsdStagePtr stage)
        {
            std::vector<UsdShadeMaterial> materials;

            if (!stage)
            {
                return materials;
            }

            auto range = stage->Traverse();
            for (auto it = range.begin(); it != range.end(); ++it)
            {
                if (it->IsA<UsdShadeMaterial>())
                {
                    materials.emplace_back(*it);
                }
            }

            return materials;
        }

        void MaterialAuthor::connectShaderToMaterial(UsdShadeShader &shader, UsdShadeMaterial &material)
        {
            if (!shader || !material)
            {
                return;
            }

            // Connect the shader's surface output to the material's surface output
            UsdShadeOutput shaderOutput = shader.CreateOutput(TfToken("surface"), SdfValueTypeNames->Token);
            UsdShadeOutput materialOutput = material.CreateSurfaceOutput();
            materialOutput.ConnectToSource(shaderOutput);
        }
    }
}