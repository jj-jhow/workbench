#include <iostream>
#include <string>
#include <filesystem>
#include <cstring>
#include <memory>

#include "converters/ConverterFactory.h"
#include "converters/UpAxis.h"
#include "converters/LinearUnit.h"

using namespace converters;
namespace fs = std::filesystem;

struct Args
{
    std::string input;
    std::string output;
    UpAxis upAxis = UpAxis::Y;
    bool help = false;
};

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [OPTIONS] INPUT\n\n";
    std::cout << "An OBJ to USD converter script.\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  INPUT                 Input OBJ file\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output OUTPUT   Specify an output USD file\n";
    std::cout << "  -u, --up-axis AXIS    Specify the up axis for the exported USD stage.\n";
    std::cout << "                        Valid values: " << UpAxisParser::getValidValues() << " (default: y)\n";
    std::cout << "  -m, --meters-per-unit UNIT Specify the unit for measurements in the USD stage.\n";
    std::cout << "                        Valid values: " << LinearUnitParser::getValidValues() << " (default: meters)\n";
    std::cout << "  -h, --help           Show this help message and exit\n";
}

bool parseArgs(int argc, char *argv[], Args &args)
{
    if (argc < 2)
    {
        return false;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0)
        {
            args.help = true;
            return true;
        }
        else if (std::strcmp(argv[i], "-o") == 0 || std::strcmp(argv[i], "--output") == 0)
        {
            if (i + 1 < argc)
            {
                args.output = argv[++i];
            }
            else
            {
                std::cerr << "Error: " << argv[i] << " requires a value\n";
                return false;
            }
        }
        else if (std::strcmp(argv[i], "-u") == 0 || std::strcmp(argv[i], "--up-axis") == 0)
        {
            if (i + 1 < argc)
            {
                try
                {
                    args.upAxis = UpAxisParser::fromString(argv[++i]);
                }
                catch (const std::invalid_argument &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: " << argv[i] << " requires a value\n";
                std::cerr << "Valid values: " << UpAxisParser::getValidValues() << std::endl;
                return false;
            }
        }
        else if (argv[i][0] == '-')
        {
            std::cerr << "Error: Unknown option " << argv[i] << std::endl;
            return false;
        }
        else
        {
            // This should be the input file
            if (args.input.empty())
            {
                args.input = argv[i];
            }
            else
            {
                std::cerr << "Error: Multiple input files specified\n";
                return false;
            }
        }
    }

    if (!args.help && args.input.empty())
    {
        std::cerr << "Error: Input file is required\n";
        return false;
    }

    return true;
}

std::string generateOutputPath(const std::string &inputPath)
{
    fs::path input(inputPath);
    fs::path output = input.parent_path() / (input.stem().string() + ".usda");
    return output.string();
}

int main(int argc, char *argv[])
{
    Args args;

    if (!parseArgs(argc, argv, args))
    {
        printUsage(argv[0]);
        return 1;
    }

    if (args.help)
    {
        printUsage(argv[0]);
        return 0;
    }

    // Generate output path if not specified (similar to Python version)
    if (args.output.empty())
    {
        args.output = generateOutputPath(args.input);
    }

    // Check if input file exists
    if (!fs::exists(args.input))
    {
        std::cerr << "Error: Input file '" << args.input << "' does not exist\n";
        return 1;
    }

    // Log conversion start (similar to Python logging)
    std::cout << "Converting " << args.input << "..." << std::endl;
    std::cout << "Up axis: " << args.upAxis << std::endl;

    try
    {
        // Create filesystem paths
        fs::path inputPath(args.input);
        fs::path outputPath(args.output);

        // Determine output format from extension
        std::string outputExt = outputPath.extension().string();
        if (!outputExt.empty() && outputExt[0] == '.')
        {
            outputExt = outputExt.substr(1);
        }

        // Use ConverterFactory to get the appropriate converter
        auto converter = ConverterFactory::Instance().GetConverterFor(inputPath, outputExt);

        if (!converter)
        {
            std::string inputExt = inputPath.extension().string();
            std::cerr << "Error: No converter available for input format '" << inputExt
                      << "' to output format '" << outputExt << "'" << std::endl;
            std::cerr << "Supported conversions: .obj to .usd/.usda" << std::endl;
            return 1;
        }

        // Create options
        ConverterOptions options;
        options.upAxis = args.upAxis;

        // Perform conversion using the factory-provided converter
        bool success = converter->Convert(inputPath, outputPath, options);

        if (success)
        {
            std::cout << "Converted results output as: " << args.output << std::endl;
            std::cout << "Done." << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "Error: Conversion failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}