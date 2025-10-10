#pragma once
#include "IConverter.h"

namespace converters
{

    class FbxToUsdConverter : public IConverter
    {
    public:
        virtual bool Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const override;

    protected:
        virtual bool Extract(pxr::UsdStageRefPtr stage, const fs::path &inputPath, const fs::path &outputPath) const override;
        virtual bool Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const override;
    };

} // namespace converters
