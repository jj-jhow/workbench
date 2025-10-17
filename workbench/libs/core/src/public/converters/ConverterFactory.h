#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <filesystem>
#include "IConverter.h"

namespace converters
{

    class ConverterFactory
    {
    public:
        using Creator = std::function<std::unique_ptr<IConverter>()>;
        static ConverterFactory &Instance();
        void Register(const std::string &type, Creator creator);
        std::unique_ptr<IConverter> Create(const std::string &type);

        // Helper: get converter by input file path and output format (e.g. "usd")
        std::unique_ptr<IConverter> GetConverterFor(const std::filesystem::path &inputPath, const std::string &outputFormat);

    private:
        std::unordered_map<std::string, Creator> creators_;
        ConverterFactory() = default;
    };

    class ConverterRegistrar
    {
    public:
        ConverterRegistrar(const std::string &type, ConverterFactory::Creator creator);
    };

} // namespace converters
