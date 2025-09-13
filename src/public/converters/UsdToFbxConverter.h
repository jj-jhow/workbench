#pragma once
#include "converters/IConverter.h"

namespace converters
{

    class UsdToFbxConverter : public IConverter
    {
    public:
        bool Convert(const std::string &inputPath, const std::string &outputPath) override;
    };

} // namespace converters
