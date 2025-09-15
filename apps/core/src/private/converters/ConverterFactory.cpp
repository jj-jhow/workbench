#include "converters/ConverterFactory.h"
#include "converters/FbxToUsdConverter.h"
#include "converters/UsdToFbxConverter.h"

namespace converters
{

    ConverterFactory &ConverterFactory::Instance()
    {
        static ConverterFactory instance;
        return instance;
    }

    void ConverterFactory::Register(const std::string &type, Creator creator)
    {
        creators_[type] = std::move(creator);
    }

    std::unique_ptr<IConverter> ConverterFactory::Create(const std::string &type)
    {
        auto it = creators_.find(type);
        if (it != creators_.end())
        {
            return (it->second)();
        }
        return nullptr;
    }

    ConverterRegistrar::ConverterRegistrar(const std::string &type, ConverterFactory::Creator creator)
    {
        ConverterFactory::Instance().Register(type, std::move(creator));
    }

    ConverterRegistrar fbxToUsdReg("fbx2usd", []()
                                   { return std::make_unique<FbxToUsdConverter>(); });
    ConverterRegistrar fbxToUsdaReg("fbx2usda", []()
                                    { return std::make_unique<FbxToUsdConverter>(); });
    ConverterRegistrar usdToFbxReg("usd2fbx", []()
                                   { return std::make_unique<UsdToFbxConverter>(); });
    ConverterRegistrar usdaToFbxReg("usda2fbx", []()
                                    { return std::make_unique<UsdToFbxConverter>(); });

    // Helper: get file extension (lowercase, no dot)
    static std::string GetFileExtension(const std::string &path)
    {
        auto pos = path.find_last_of('.');
        if (pos == std::string::npos || pos == path.length() - 1)
            return "";
        std::string ext = path.substr(pos + 1);
        for (auto &c : ext)
            c = static_cast<char>(tolower(c));
        return ext;
    }

    std::unique_ptr<IConverter> ConverterFactory::GetConverterFor(const std::string &inputPath, const std::string &outputFormat)
    {
        std::string inputExt = GetFileExtension(inputPath);
        if (inputExt.empty() || outputFormat.empty())
            return nullptr;
        std::string type = inputExt + "2" + outputFormat;
        return Create(type);
    }

}
