#pragma once
#include "IConverter.h"

namespace converters
{

    class UsdToFbxConverter : public IConverter
    {
    public:
        virtual bool Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const override;

    protected:
        virtual pxr::UsdStageRefPtr Extract(const fs::path &inputPath, const fs::path &outputPath) const override;
        virtual void Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const override;
    };

} // namespace converters
