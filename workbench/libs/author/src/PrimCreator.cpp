#include "PrimCreator.h"
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdLux/distantLight.h>
#include <pxr/usd/usdLux/sphereLight.h>
#include <pxr/usd/usdLux/rectLight.h>
#include <pxr/base/tf/stringUtils.h>
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace author
    {
        UsdPrim PrimCreator::createXform(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdPrim();
            }

            UsdGeomXform xform = UsdGeomXform::Define(stage, SdfPath(primPath));
            setDefaultAttributes(xform.GetPrim());
            return xform.GetPrim();
        }

        UsdPrim PrimCreator::createScope(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdPrim();
            }

            UsdGeomScope scope = UsdGeomScope::Define(stage, SdfPath(primPath));
            setDefaultAttributes(scope.GetPrim());
            return scope.GetPrim();
        }

        UsdGeomMesh PrimCreator::createMesh(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdGeomMesh();
            }

            UsdGeomMesh mesh = UsdGeomMesh::Define(stage, SdfPath(primPath));
            setDefaultAttributes(mesh.GetPrim());
            return mesh;
        }

        UsdGeomSphere PrimCreator::createSphere(UsdStagePtr stage, const std::string &primPath, double radius)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdGeomSphere();
            }

            UsdGeomSphere sphere = UsdGeomSphere::Define(stage, SdfPath(primPath));
            sphere.CreateRadiusAttr().Set(radius);
            setDefaultAttributes(sphere.GetPrim());
            return sphere;
        }

        UsdGeomCube PrimCreator::createCube(UsdStagePtr stage, const std::string &primPath, double size)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdGeomCube();
            }

            UsdGeomCube cube = UsdGeomCube::Define(stage, SdfPath(primPath));
            cube.CreateSizeAttr().Set(size);
            setDefaultAttributes(cube.GetPrim());
            return cube;
        }

        UsdGeomCylinder PrimCreator::createCylinder(UsdStagePtr stage, const std::string &primPath, double radius, double height)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdGeomCylinder();
            }

            UsdGeomCylinder cylinder = UsdGeomCylinder::Define(stage, SdfPath(primPath));
            cylinder.CreateRadiusAttr().Set(radius);
            cylinder.CreateHeightAttr().Set(height);
            setDefaultAttributes(cylinder.GetPrim());
            return cylinder;
        }

        UsdGeomPlane PrimCreator::createPlane(UsdStagePtr stage, const std::string &primPath, double width, double length)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdGeomPlane();
            }

            UsdGeomPlane plane = UsdGeomPlane::Define(stage, SdfPath(primPath));
            plane.CreateWidthAttr().Set(width);
            plane.CreateLengthAttr().Set(length);
            setDefaultAttributes(plane.GetPrim());
            return plane;
        }

        UsdGeomCamera PrimCreator::createCamera(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdGeomCamera();
            }

            UsdGeomCamera camera = UsdGeomCamera::Define(stage, SdfPath(primPath));
            // Set some reasonable default camera parameters
            camera.CreateFocalLengthAttr().Set(50.0f);
            camera.CreateHorizontalApertureAttr().Set(36.0f);
            camera.CreateVerticalApertureAttr().Set(24.0f);
            setDefaultAttributes(camera.GetPrim());
            return camera;
        }

        UsdPrim PrimCreator::createDistantLight(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdPrim();
            }

            UsdLuxDistantLight light = UsdLuxDistantLight::Define(stage, SdfPath(primPath));
            light.CreateIntensityAttr().Set(1.0f);
            setDefaultAttributes(light.GetPrim());
            return light.GetPrim();
        }

        UsdPrim PrimCreator::createSphereLight(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdPrim();
            }

            UsdLuxSphereLight light = UsdLuxSphereLight::Define(stage, SdfPath(primPath));
            light.CreateIntensityAttr().Set(1.0f);
            light.CreateRadiusAttr().Set(0.5f);
            setDefaultAttributes(light.GetPrim());
            return light.GetPrim();
        }

        UsdPrim PrimCreator::createRectLight(UsdStagePtr stage, const std::string &primPath)
        {
            if (!validatePrimPath(stage, primPath))
            {
                return UsdPrim();
            }

            UsdLuxRectLight light = UsdLuxRectLight::Define(stage, SdfPath(primPath));
            light.CreateIntensityAttr().Set(1.0f);
            light.CreateWidthAttr().Set(1.0f);
            light.CreateHeightAttr().Set(1.0f);
            setDefaultAttributes(light.GetPrim());
            return light.GetPrim();
        }

        bool PrimCreator::validatePrimPath(UsdStagePtr stage, const std::string &primPath) const
        {
            if (!stage)
            {
                std::cerr << "Invalid stage provided" << std::endl;
                return false;
            }

            if (primPath.empty() || primPath[0] != '/')
            {
                std::cerr << "Invalid prim path: " << primPath << std::endl;
                return false;
            }

            SdfPath path(primPath);
            if (!path.IsAbsolutePath())
            {
                std::cerr << "Invalid prim path format: " << primPath << std::endl;
                return false;
            }

            return true;
        }

        std::string PrimCreator::generateUniquePrimPath(UsdStagePtr stage, const std::string &basePath) const
        {
            if (!stage)
            {
                return "";
            }

            std::string uniquePath = basePath;
            int counter = 1;

            while (stage->GetPrimAtPath(SdfPath(uniquePath)))
            {
                uniquePath = basePath + TfStringPrintf("_%d", counter);
                counter++;
            }

            return uniquePath;
        }

        void PrimCreator::setDefaultAttributes(UsdPrim prim) const
        {
            if (!prim)
            {
                return;
            }

            // Set any common default attributes here
            // For example, you might want to set default visibility, purpose, etc.
            // UsdGeomImageable imageable(prim);
            // if (imageable) {
            //     imageable.CreateVisibilityAttr().Set(UsdGeomTokens->inherited);
            // }
        }
    }
}