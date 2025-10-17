#include "converters/LinearUnit.h"

namespace converters
{

    // Static member definition
    const std::unordered_map<std::string, LinearUnit> LinearUnitParser::stringToEnum =
        {
            {"millimeters", LinearUnit::Millimeters},
            {"centimeters", LinearUnit::Centimeters},
            {"meters", LinearUnit::Meters},
            {"kilometers", LinearUnit::Kilometers},
            {"inches", LinearUnit::Inches},
            {"feet", LinearUnit::Feet},
            {"yards", LinearUnit::Yards},
            {"miles", LinearUnit::Miles}};
} // namespace converters