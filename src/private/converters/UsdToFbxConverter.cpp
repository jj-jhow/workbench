#include "converters/UsdToFbxConverter.h"
#include <iostream>

namespace converters
{

    bool UsdToFbxConverter::Convert(const std::string &inputPath, const std::string &outputPath)
    {
        std::cout << "Converting USD to FBX: " << inputPath << " -> " << outputPath << std::endl;
        // TODO: Implement actual USD to FBX conversion logic
        return false;
    }

} // namespace converters
