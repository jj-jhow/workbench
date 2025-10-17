#include "converters/UpAxis.h"

namespace converters
{

    // Static member definition
    const std::unordered_map<std::string, UpAxis> UpAxisParser::stringToEnum =
        {
            {"y", UpAxis::Y},
            {"z", UpAxis::Z}};

} // namespace converters