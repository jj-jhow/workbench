#include "converters/FbxToUsdConverter.h"

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/metrics.h>

#include <iostream>

namespace converters
{

    bool FbxToUsdConverter::Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const
    {
        std::cout << "Converting FBX to USD: " << inputPath << " -> " << outputPath << std::endl;
        // Placeholder for actual conversion logic
        return true;
    }

    bool FbxToUsdConverter::Extract(pxr::UsdStageRefPtr stage, const fs::path &inputPath, const fs::path &outputPath) const
    {
        std::cout << "Extracting data from: " << inputPath << " to " << outputPath << std::endl;
        // Placeholder for extraction logic
        return false;
    }

    bool FbxToUsdConverter::Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const
    {
        std::cout << "Transforming stage with up axis: " << UpAxisParser::toString(options.upAxis) << std::endl;
        return true;
        // Placeholder for transformation logic
    }
} // namespace converters
