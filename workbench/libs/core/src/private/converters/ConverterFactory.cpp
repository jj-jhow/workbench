#include "converters/ConverterFactory.h"

#include "converters/FbxToUsdConverter.h"
#include "converters/UsdToFbxConverter.h"
#include "converters/ObjToUsdConverter.h"

#include <filesystem>
#include <cctype>

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
    ConverterRegistrar objToUsdReg("obj2usd", []()
                                   { return std::make_unique<ObjToUsdConverter>(); });
    ConverterRegistrar objToUsdaReg("obj2usda", []()
                                    { return std::make_unique<ObjToUsdConverter>(); });

    // Helper: get file extension (lowercase, no dot)
    static std::string GetFileExtension(const std::string &path)
    {
        std::filesystem::path fsPath(path);
        std::string ext = fsPath.extension().string();

        // Remove the leading dot and convert to lowercase
        if (!ext.empty() && ext[0] == '.')
        {
            ext = ext.substr(1);
        }

        // Convert to lowercase
        for (auto &c : ext)
        {
            c = static_cast<char>(std::tolower(c));
        }

        return ext;
    }

    std::unique_ptr<IConverter> ConverterFactory::GetConverterFor(const std::filesystem::path &inputPath, const std::string &outputFormat)
    {
        std::string inputExt = inputPath.extension().string();

        // Remove the leading dot and convert to lowercase
        if (!inputExt.empty() && inputExt[0] == '.')
        {
            inputExt = inputExt.substr(1);
        }

        // Convert to lowercase
        for (auto &c : inputExt)
        {
            c = static_cast<char>(std::tolower(c));
        }

        if (inputExt.empty() || outputFormat.empty())
            return nullptr;

        std::string type = inputExt + "2" + outputFormat;
        return Create(type);
    }

}
