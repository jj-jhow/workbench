#pragma once

#include "IConverter.h"

struct aiMesh;
struct aiScene;
struct aiMaterial;

namespace converters
{
    class ObjToUsdConverter : public IConverter
    {
    public:
        virtual bool Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const override;

    protected:
        virtual pxr::UsdStageRefPtr Extract(const fs::path &inputPath, const fs::path &outputPath) const override;
        virtual void Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const override;

    private:
        void ExtractMeshData(const aiMesh *mesh, const aiScene *scene, pxr::UsdStageRefPtr stage) const;
        pxr::UsdShadeMaterial ExtractMaterialData(const aiMaterial *material, pxr::UsdStageRefPtr stage) const;

        bool SetDefaultPrim(pxr::UsdStageRefPtr stage) const;
        bool SetUpAxis(pxr::UsdStageRefPtr stage, UpAxis upAxis) const;
        bool SetMetersPerUnit(pxr::UsdStageRefPtr stage, LinearUnit linearUnit) const;
    };

} // namespace converters
