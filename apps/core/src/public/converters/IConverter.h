#pragma once

#include "UpAxis.h"

#include <pxr/usd/usd/stage.h>

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace converters
{
    struct ConverterOptions
    {
        UpAxis upAxis = UpAxis::Y;

        virtual ~ConverterOptions() = default;
    };

    class IConverter
    {
    public:
        virtual ~IConverter() = default;
        virtual bool Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const = 0;

    protected:
        virtual pxr::UsdStageRefPtr Extract(const fs::path &inputPath, const fs::path &outputPath) const = 0;
        virtual void Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const = 0;
    };

} // namespace converters
