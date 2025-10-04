#pragma once

#include "IConverter.h"

struct aiMesh;

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
        void ConvertMeshToUsd(const ::aiMesh *mesh, pxr::UsdStageRefPtr stage) const;
    };

} // namespace converters
