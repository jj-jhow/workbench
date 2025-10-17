#include "AttributeEditor.h"
#include <pxr/usd/usd/tokens.h>
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace author
    {
        UsdAttribute AttributeEditor::createAttribute(UsdPrim &prim, const TfToken &attrName, const SdfValueTypeName &typeName)
        {
            if (!prim)
            {
                std::cerr << "Invalid prim provided" << std::endl;
                return UsdAttribute();
            }

            if (!isValidAttributeName(attrName))
            {
                std::cerr << "Invalid attribute name: " << attrName << std::endl;
                return UsdAttribute();
            }

            return prim.CreateAttribute(attrName, typeName);
        }

        bool AttributeEditor::setAttribute(UsdPrim &prim, const TfToken &attrName, const VtValue &value, UsdTimeCode time)
        {
            if (!prim)
            {
                return false;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                std::cerr << "Attribute does not exist: " << attrName << std::endl;
                return false;
            }

            return attr.Set(value, time);
        }

        bool AttributeEditor::setAttributeAtTime(UsdPrim &prim, const TfToken &attrName, const VtValue &value, double timeCode)
        {
            return setAttribute(prim, attrName, value, UsdTimeCode(timeCode));
        }

        VtValue AttributeEditor::getAttribute(UsdPrim &prim, const TfToken &attrName, UsdTimeCode time)
        {
            if (!prim)
            {
                return VtValue();
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return VtValue();
            }

            VtValue value;
            attr.Get(&value, time);
            return value;
        }

        std::vector<UsdAttribute> AttributeEditor::getAllAttributes(UsdPrim &prim)
        {
            std::vector<UsdAttribute> attributes;

            if (!prim)
            {
                return attributes;
            }

            attributes = prim.GetAttributes();
            return attributes;
        }

        std::vector<UsdAttribute> AttributeEditor::getCustomAttributes(UsdPrim &prim)
        {
            std::vector<UsdAttribute> customAttributes;

            if (!prim)
            {
                return customAttributes;
            }

            std::vector<UsdAttribute> allAttributes = prim.GetAttributes();
            for (const auto &attr : allAttributes)
            {
                if (attr.IsCustom())
                {
                    customAttributes.push_back(attr);
                }
            }

            return customAttributes;
        }

        bool AttributeEditor::hasAttribute(UsdPrim &prim, const TfToken &attrName)
        {
            if (!prim)
            {
                return false;
            }

            return prim.HasAttribute(attrName);
        }

        SdfValueTypeName AttributeEditor::getAttributeTypeName(UsdPrim &prim, const TfToken &attrName)
        {
            if (!prim)
            {
                return SdfValueTypeName();
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return SdfValueTypeName();
            }

            return attr.GetTypeName();
        }

        bool AttributeEditor::isAttributeAnimated(UsdPrim &prim, const TfToken &attrName)
        {
            if (!prim)
            {
                return false;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return false;
            }

            std::vector<double> timeSamples;
            return attr.GetTimeSamples(&timeSamples) && timeSamples.size() > 1;
        }

        bool AttributeEditor::setAttributeMetadata(UsdPrim &prim, const TfToken &attrName, const TfToken &key, const VtValue &value)
        {
            if (!prim)
            {
                return false;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return false;
            }

            return attr.SetMetadata(key, value);
        }

        VtValue AttributeEditor::getAttributeMetadata(UsdPrim &prim, const TfToken &attrName, const TfToken &key)
        {
            if (!prim)
            {
                return VtValue();
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return VtValue();
            }

            VtValue value;
            attr.GetMetadata(key, &value);
            return value;
        }

        bool AttributeEditor::setTimeSample(UsdPrim &prim, const TfToken &attrName, double timeCode, const VtValue &value)
        {
            if (!prim)
            {
                return false;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return false;
            }

            return attr.Set(value, UsdTimeCode(timeCode));
        }

        std::vector<double> AttributeEditor::getTimeSamples(UsdPrim &prim, const TfToken &attrName)
        {
            std::vector<double> timeSamples;

            if (!prim)
            {
                return timeSamples;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return timeSamples;
            }

            attr.GetTimeSamples(&timeSamples);
            return timeSamples;
        }

        bool AttributeEditor::clearTimeSamples(UsdPrim &prim, const TfToken &attrName)
        {
            if (!prim)
            {
                return false;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return false;
            }

            return attr.ClearAtTime(UsdTimeCode::Default());
        }

        bool AttributeEditor::validateAttributeValue(const VtValue &value, const SdfValueTypeName &expectedType)
        {
            if (value.IsEmpty())
            {
                return false;
            }

            // Simple type checking - in a real implementation you'd want more sophisticated validation
            return value.GetType() == expectedType.GetType();
        }

        std::vector<std::string> AttributeEditor::validatePrimAttributes(UsdPrim &prim)
        {
            std::vector<std::string> issues;

            if (!prim)
            {
                issues.push_back("Invalid prim");
                return issues;
            }

            std::vector<UsdAttribute> attributes = prim.GetAttributes();
            for (const auto &attr : attributes)
            {
                // Check if attribute has a valid value
                VtValue value;
                if (!attr.Get(&value) || value.IsEmpty())
                {
                    issues.push_back("Attribute '" + attr.GetName().GetString() + "' has no value");
                }

                // Check type consistency
                SdfValueTypeName typeName = attr.GetTypeName();
                if (!validateAttributeValue(value, typeName))
                {
                    issues.push_back("Attribute '" + attr.GetName().GetString() + "' has incorrect type");
                }
            }

            return issues;
        }

        bool AttributeEditor::removeAttribute(UsdPrim &prim, const TfToken &attrName)
        {
            if (!prim)
            {
                return false;
            }

            UsdAttribute attr = prim.GetAttribute(attrName);
            if (!attr)
            {
                return false;
            }

            return prim.RemoveProperty(attrName);
        }

        bool AttributeEditor::copyAttribute(UsdPrim &sourcePrim, UsdPrim &targetPrim, const TfToken &attrName)
        {
            if (!sourcePrim || !targetPrim)
            {
                return false;
            }

            UsdAttribute sourceAttr = sourcePrim.GetAttribute(attrName);
            if (!sourceAttr)
            {
                return false;
            }

            // Create the attribute on target prim
            UsdAttribute targetAttr = targetPrim.CreateAttribute(attrName, sourceAttr.GetTypeName());
            if (!targetAttr)
            {
                return false;
            }

            // Copy the value
            VtValue value;
            if (sourceAttr.Get(&value))
            {
                targetAttr.Set(value);
            }

            // Copy time samples if any
            std::vector<double> timeSamples;
            if (sourceAttr.GetTimeSamples(&timeSamples))
            {
                for (double time : timeSamples)
                {
                    if (sourceAttr.Get(&value, UsdTimeCode(time)))
                    {
                        targetAttr.Set(value, UsdTimeCode(time));
                    }
                }
            }

            return true;
        }

        bool AttributeEditor::isValidAttributeName(const TfToken &attrName) const
        {
            if (attrName.IsEmpty())
            {
                return false;
            }

            std::string name = attrName.GetString();

            // Check if it starts with a letter or underscore
            if (name.empty() || (!std::isalpha(name[0]) && name[0] != '_'))
            {
                return false;
            }

            // Check if all characters are alphanumeric or underscore
            for (char c : name)
            {
                if (!std::isalnum(c) && c != '_')
                {
                    return false;
                }
            }

            return true;
        }
    }
}