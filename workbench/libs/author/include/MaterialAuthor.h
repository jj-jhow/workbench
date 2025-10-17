#pragma once

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdShade/input.h>
#include <pxr/usd/usdShade/output.h>
#include <pxr/base/gf/vec3f.h>
#include <string>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace author
    {
        /**
         * @brief Material and shading authoring utilities
         *
         * This class provides tools for creating and editing USD materials,
         * shading networks, and material assignments.
         */
        class MaterialAuthor
        {
        public:
            MaterialAuthor() = default;
            ~MaterialAuthor() = default;

            // Material creation
            UsdShadeMaterial createMaterial(UsdStagePtr stage, const std::string &materialPath);
            UsdShadeMaterial createBasicMaterial(UsdStagePtr stage, const std::string &materialPath,
                                                 const GfVec3f &diffuseColor = GfVec3f(0.5f, 0.5f, 0.5f));
            UsdShadeMaterial createPBRMaterial(UsdStagePtr stage, const std::string &materialPath);

            // Shader creation and connection
            UsdShadeShader createPreviewSurfaceShader(UsdShadeMaterial &material, const std::string &shaderName = "PreviewSurface");
            UsdShadeShader createTextureShader(UsdShadeMaterial &material, const std::string &texturePath, const std::string &shaderName = "DiffuseTexture");

            // Material parameter setting
            void setDiffuseColor(UsdShadeShader &shader, const GfVec3f &color);
            void setMetallic(UsdShadeShader &shader, float metallic);
            void setRoughness(UsdShadeShader &shader, float roughness);
            void setOpacity(UsdShadeShader &shader, float opacity);
            void setEmissiveColor(UsdShadeShader &shader, const GfVec3f &color);

            // Material assignment
            bool assignMaterial(UsdPrim &prim, UsdShadeMaterial &material);
            bool clearMaterialAssignment(UsdPrim &prim);

            // Utility methods
            UsdShadeMaterial getMaterialFromPrim(UsdPrim &prim);
            std::vector<UsdShadeMaterial> getAllMaterials(UsdStagePtr stage);

        private:
            void connectShaderToMaterial(UsdShadeShader &shader, UsdShadeMaterial &material);
        };
    }
}