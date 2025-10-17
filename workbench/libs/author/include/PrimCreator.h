#pragma once

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/plane.h>

#include <pxr/usd/usdGeom/camera.h>
#include <string>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace author
    {
        /**
         * @brief Factory class for creating USD primitives
         *
         * This class provides convenient methods for creating various types of USD primitives
         * with sensible defaults and proper initialization.
         */
        class PrimCreator
        {
        public:
            PrimCreator() = default;
            ~PrimCreator() = default;

            // Basic primitive creation
            UsdPrim createXform(UsdStagePtr stage, const std::string &primPath);
            UsdPrim createScope(UsdStagePtr stage, const std::string &primPath);

            // Geometry primitives
            UsdGeomMesh createMesh(UsdStagePtr stage, const std::string &primPath);
            UsdGeomSphere createSphere(UsdStagePtr stage, const std::string &primPath, double radius = 1.0);
            UsdGeomCube createCube(UsdStagePtr stage, const std::string &primPath, double size = 1.0);
            UsdGeomCylinder createCylinder(UsdStagePtr stage, const std::string &primPath, double radius = 0.5, double height = 1.0);
            UsdGeomPlane createPlane(UsdStagePtr stage, const std::string &primPath, double width = 1.0, double length = 1.0);

            // Cameras and lights
            UsdGeomCamera createCamera(UsdStagePtr stage, const std::string &primPath);
            UsdPrim createDistantLight(UsdStagePtr stage, const std::string &primPath);
            UsdPrim createSphereLight(UsdStagePtr stage, const std::string &primPath);
            UsdPrim createRectLight(UsdStagePtr stage, const std::string &primPath);

            // Utility methods
            bool validatePrimPath(UsdStagePtr stage, const std::string &primPath) const;
            std::string generateUniquePrimPath(UsdStagePtr stage, const std::string &basePath) const;

        private:
            void setDefaultAttributes(UsdPrim prim) const;
        };
    }
}