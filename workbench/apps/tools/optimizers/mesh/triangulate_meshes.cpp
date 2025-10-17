#include <iostream>
#include <string>
#include <pxr/usd/usd/stage.h>
#include <pxr/base/tf/stringUtils.h>
#include "MeshTriangulator.h"

PXR_NAMESPACE_USING_DIRECTIVE

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [options] input_file [output_file]\n\n";
    std::cout << "Triangulate all n-gon faces in USD meshes to triangles.\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  input_file              USD file to triangulate (.usd, .usda, .usdc)\n";
    std::cout << "  output_file             Output USD file (optional, defaults to input_triangulated.usd)\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  --in-place              Modify the input file directly (ignores output_file)\n";
    std::cout << "  --no-primvars           Don't preserve primvar data during triangulation\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " scene.usd\n";
    std::cout << "  " << programName << " -v scene.usd triangulated_scene.usd\n";
    std::cout << "  " << programName << " --in-place scene.usd\n";
}

int main(int argc, char *argv[])
{
    std::string inputFile;
    std::string outputFile;
    bool verbose = false;
    bool inPlace = false;
    bool preservePrimvars = true;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-v" || arg == "--verbose")
        {
            verbose = true;
        }
        else if (arg == "--in-place")
        {
            inPlace = true;
        }
        else if (arg == "--no-primvars")
        {
            preservePrimvars = false;
        }
        else if (inputFile.empty())
        {
            inputFile = arg;
        }
        else if (outputFile.empty())
        {
            outputFile = arg;
        }
        else
        {
            std::cerr << "Error: Too many arguments" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    if (inputFile.empty())
    {
        std::cerr << "Error: Input file is required" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // Generate output filename if not provided and not in-place
    if (outputFile.empty() && !inPlace)
    {
        size_t lastDot = inputFile.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            outputFile = inputFile.substr(0, lastDot) + "_triangulated" + inputFile.substr(lastDot);
        }
        else
        {
            outputFile = inputFile + "_triangulated";
        }
    }

    if (verbose)
    {
        std::cout << "Input file: " << inputFile << std::endl;
        if (inPlace)
        {
            std::cout << "Mode: In-place modification" << std::endl;
        }
        else
        {
            std::cout << "Output file: " << outputFile << std::endl;
        }
        std::cout << "Preserve primvars: " << (preservePrimvars ? "Yes" : "No") << std::endl;
    }

    // Open the USD stage
    UsdStageRefPtr stage = UsdStage::Open(inputFile);
    if (!stage)
    {
        std::cerr << "Error: Failed to open USD file: " << inputFile << std::endl;
        return 1;
    }

    // Set up triangulation options
    workbench::optimizer::MeshTriangulator::TriangulationOptions options;
    options.verbose = verbose;
    options.inPlace = inPlace;
    options.preserveOriginalPrimvars = preservePrimvars;

    // Create triangulator and process the stage
    workbench::optimizer::MeshTriangulator triangulator(options);

    if (verbose)
    {
        std::cout << "Starting triangulation..." << std::endl;
    }

    bool success = triangulator.triangulateStage(stage);

    if (!success)
    {
        std::cerr << "Error: Triangulation failed" << std::endl;
        return 1;
    }

    // Save the result
    std::string saveFile = inPlace ? inputFile : outputFile;

    if (verbose)
    {
        std::cout << "Saving to: " << saveFile << std::endl;
    }

    if (!stage->Export(saveFile))
    {
        std::cerr << "Error: Failed to save USD file: " << saveFile << std::endl;
        return 1;
    }

    // Print statistics
    const auto &stats = triangulator.getStats();
    std::cout << "Triangulation complete!" << std::endl;
    std::cout << "Meshes processed: " << stats.meshesProcessed << std::endl;
    std::cout << "Faces triangulated: " << stats.facesTriangulated << std::endl;
    std::cout << "Original face count: " << stats.originalFaceCount << std::endl;
    std::cout << "Final face count: " << stats.finalFaceCount << std::endl;

    return 0;
}