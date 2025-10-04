#pragma once

#include <pxr/usd/usdGeom/tokens.h>

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <vector>

namespace converters
{

    enum class UpAxis
    {
        X,
        Y,
        Z
    };

    class UpAxisParser
    {
    public:
        // Convert string to UpAxis enum (similar to Python's type conversion)
        static UpAxis fromString(const std::string &str)
        {
            std::string lowerStr = toLower(str);

            auto it = stringToEnum.find(lowerStr);
            if (it != stringToEnum.end())
            {
                return it->second;
            }

            throw std::invalid_argument("Invalid up-axis value: '" + str + "'. Valid values are: x, y, z");
        }

        // Convert UpAxis enum to USD token (equivalent to enum.value in Python)
        static const pxr::TfToken &toToken(UpAxis axis)
        {
            switch (axis)
            {
            case UpAxis::Y:
                return pxr::UsdGeomTokens->y;
            case UpAxis::Z:
                return pxr::UsdGeomTokens->z;
            default:
                throw std::runtime_error("Unknown UpAxis value");
            }
        }

        // Direct string to token conversion (convenience method)
        static const pxr::TfToken &stringToToken(const std::string &str)
        {
            return toToken(fromString(str));
        }

        // Convert UpAxis to string (equivalent to Python's __str__)
        static std::string toString(UpAxis axis)
        {
            return toToken(axis).GetString();
        }

        // Get list of valid values for help text
        static std::string getValidValues()
        {
            return "x, y, z";
        }

        // Get all enum values (similar to Python's list(UpAxis))
        static std::vector<UpAxis> getAllValues()
        {
            return {UpAxis::X, UpAxis::Y, UpAxis::Z};
        }

    private:
        static std::string toLower(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        static const std::unordered_map<std::string, UpAxis> stringToEnum;
    };

    // Stream operator for easy printing (similar to Python's str())
    inline std::ostream &operator<<(std::ostream &os, UpAxis axis)
    {
        os << UpAxisParser::toString(axis);
        return os;
    }

} // namespace converters