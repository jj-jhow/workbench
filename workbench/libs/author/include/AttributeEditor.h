#pragma once

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/base/vt/value.h>
#include <pxr/base/tf/token.h>
#include <string>
#include <vector>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace author
    {
        /**
         * @brief Generic attribute editing and validation utilities
         *
         * This class provides tools for manipulating USD attributes with
         * type safety and validation.
         */
        class AttributeEditor
        {
        public:
            AttributeEditor() = default;
            ~AttributeEditor() = default;

            // Attribute creation and modification
            UsdAttribute createAttribute(UsdPrim &prim, const TfToken &attrName, const SdfValueTypeName &typeName);
            bool setAttribute(UsdPrim &prim, const TfToken &attrName, const VtValue &value, UsdTimeCode time = UsdTimeCode::Default());
            bool setAttributeAtTime(UsdPrim &prim, const TfToken &attrName, const VtValue &value, double timeCode);

            // Attribute retrieval
            VtValue getAttribute(UsdPrim &prim, const TfToken &attrName, UsdTimeCode time = UsdTimeCode::Default());
            std::vector<UsdAttribute> getAllAttributes(UsdPrim &prim);
            std::vector<UsdAttribute> getCustomAttributes(UsdPrim &prim);

            // Attribute queries
            bool hasAttribute(UsdPrim &prim, const TfToken &attrName);
            SdfValueTypeName getAttributeTypeName(UsdPrim &prim, const TfToken &attrName);
            bool isAttributeAnimated(UsdPrim &prim, const TfToken &attrName);

            // Attribute metadata
            bool setAttributeMetadata(UsdPrim &prim, const TfToken &attrName, const TfToken &key, const VtValue &value);
            VtValue getAttributeMetadata(UsdPrim &prim, const TfToken &attrName, const TfToken &key);

            // Animation and time samples
            bool setTimeSample(UsdPrim &prim, const TfToken &attrName, double timeCode, const VtValue &value);
            std::vector<double> getTimeSamples(UsdPrim &prim, const TfToken &attrName);
            bool clearTimeSamples(UsdPrim &prim, const TfToken &attrName);

            // Validation
            bool validateAttributeValue(const VtValue &value, const SdfValueTypeName &expectedType);
            std::vector<std::string> validatePrimAttributes(UsdPrim &prim);

            // Utility methods
            bool removeAttribute(UsdPrim &prim, const TfToken &attrName);
            bool copyAttribute(UsdPrim &sourcePrim, UsdPrim &targetPrim, const TfToken &attrName);

        private:
            bool isValidAttributeName(const TfToken &attrName) const;
        };
    }
}