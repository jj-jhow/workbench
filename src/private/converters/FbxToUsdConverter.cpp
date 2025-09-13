#include "converters/FbxToUsdConverter.h"
#include <iostream>

namespace converters
{

    bool FbxToUsdConverter::Convert(const std::string &inputPath, const std::string &outputPath)
    {
        std::cout << "Converting FBX to USD: " << inputPath << " -> " << outputPath << std::endl;
        // TODO: Implement actual FBX to USD conversion logic
        return false;
    }

} // namespace converters
