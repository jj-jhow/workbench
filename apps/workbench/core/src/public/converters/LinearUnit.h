#pragma once

#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/metrics.h>

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <vector>

namespace converters
{

    enum class LinearUnit
    {
        Millimeters,
        Centimeters,
        Meters,
        Kilometers,
        Inches,
        Feet,
        Yards,
        Miles
    };

    class LinearUnitParser
    {
    public:
        // Convert string to LinearUnit enum (similar to Python's type conversion)
        static LinearUnit fromString(const std::string &str)
        {
            std::string lowerStr = toLower(str);

            auto it = stringToEnum.find(lowerStr);
            if (it != stringToEnum.end())
            {
                return it->second;
            }

            throw std::invalid_argument("Invalid linear unit value: '" + str + "'. Valid values are: millimeters, centimeters, meters, kilometers, inches, feet, yards, miles");
        }

        // Convert LinearUnit enum to double (equivalent to enum.value in Python)
        static constexpr inline double toDouble(LinearUnit unit)
        {
            switch (unit)
            {
            case LinearUnit::Millimeters:
                return pxr::UsdGeomLinearUnits::millimeters;
            case LinearUnit::Centimeters:
                return pxr::UsdGeomLinearUnits::centimeters;
            case LinearUnit::Meters:
                return pxr::UsdGeomLinearUnits::meters;
            case LinearUnit::Kilometers:
                return pxr::UsdGeomLinearUnits::kilometers;
            case LinearUnit::Inches:
                return pxr::UsdGeomLinearUnits::inches;
            case LinearUnit::Feet:
                return pxr::UsdGeomLinearUnits::feet;
            case LinearUnit::Yards:
                return pxr::UsdGeomLinearUnits::yards;
            case LinearUnit::Miles:
                return pxr::UsdGeomLinearUnits::miles;
            default:
                throw std::runtime_error("Unknown LinearUnit value");
            }
        }

        // Direct string to double conversion (convenience method)
        static const double &stringToDouble(const std::string &str)
        {
            return toDouble(fromString(str));
        }

        // Convert LinearUnit to string (equivalent to Python's __str__)
        static std::string toString(LinearUnit unit)
        {
            return std::to_string(toDouble(unit));
        }

        // Get list of valid values for help text
        static std::string getValidValues()
        {
            return "millimeters, centimeters, meters, kilometers, inches, feet, yards, miles";
        }

        // Get all enum values (similar to Python's list(LinearUnit))
        static std::vector<LinearUnit> getAllValues()
        {
            return {LinearUnit::Millimeters, LinearUnit::Centimeters, LinearUnit::Meters, LinearUnit::Kilometers, LinearUnit::Inches, LinearUnit::Feet, LinearUnit::Yards, LinearUnit::Miles};
        }

    private:
        static std::string toLower(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        static const std::unordered_map<std::string, LinearUnit> stringToEnum;
    };

    // Stream operator for easy printing (similar to Python's str())
    inline std::ostream &operator<<(std::ostream &os, LinearUnit unit)
    {
        os << LinearUnitParser::toString(unit);
        return os;
    }

} // namespace converters