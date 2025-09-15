#pragma once
#include <string>

namespace converters
{

    class IConverter
    {
    public:
        virtual ~IConverter() = default;
        virtual bool Convert(const std::string &inputPath, const std::string &outputPath) = 0;
    };

} // namespace converters
