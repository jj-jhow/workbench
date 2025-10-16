#include <iostream>
#include <string>
#include <iomanip>
#include <pxr/usd/usd/stage.h>
#include <pxr/base/tf/stringUtils.h>
#include "HiddenMeshRemover.h"

PXR_NAMESPACE_USING_DIRECTIVE

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [options] input_file [output_file]\n\n";
    std::cout << "Optimize USD files by setting visibility=invisible for hidden meshes.\n";
    std::cout << "Uses USD's non-destructive approach - original meshes remain in the file.\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  input_file              USD file to optimize (.usd, .usda, .usdc)\n";
    std::cout << "  output_file             Output USD file (optional, defaults to input_optimized.usd)\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  --dry-run               Analyze without modifying visibility\n";
    std::cout << "  --in-place              Modify the input file directly (ignores output_file)\n";
    std::cout << "  --no-cameras            Don't use existing camera viewpoints\n";
    std::cout << "  --no-generate           Don't generate additional viewpoints\n";
    std::cout << "  --viewpoint-density N   Number of viewpoints per axis (default: 8)\n";
    std::cout << "  --occlusion-threshold T Occlusion threshold 0.0-1.0 (default: 0.95)\n";
    std::cout << "  --aggressive            Use aggressive hiding (less conservative)\n";
    std::cout << "  --preserve-instanced    Don't hide instanced meshes (default: true)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " scene.usd\n";
    std::cout << "  " << programName << " -v --dry-run scene.usd\n";
    std::cout << "  " << programName << " --aggressive scene.usd optimized_scene.usd\n";
    std::cout << "  " << programName << " --viewpoint-density 12 --in-place scene.usd\n";
}

int main(int argc, char *argv[])
{
    std::string inputFile;
    std::string outputFile;
    bool dryRun = false;
    bool inPlace = false;

    // Initialize options with defaults
    workbench::optimizer::HiddenMeshRemover::RemovalOptions options;

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
            options.verbose = true;
        }
        else if (arg == "--dry-run")
        {
            dryRun = true;
        }
        else if (arg == "--in-place")
        {
            inPlace = true;
        }
        else if (arg == "--no-cameras")
        {
            options.useExistingCameras = false;
        }
        else if (arg == "--no-generate")
        {
            options.generateViewpoints = false;
        }
        else if (arg == "--aggressive")
        {
            options.conservativeRemoval = false;
            options.occlusionThreshold = 0.8f;
        }
        else if (arg == "--preserve-instanced")
        {
            options.preserveInstancedMeshes = true;
        }
        else if (arg == "--viewpoint-density" && i + 1 < argc)
        {
            try
            {
                options.viewpointDensity = std::stof(argv[++i]);
                if (options.viewpointDensity < 1.0f || options.viewpointDensity > 20.0f)
                {
                    std::cerr << "Error: viewpoint-density must be between 1 and 20\n";
                    return 1;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: Invalid viewpoint-density value\n";
                return 1;
            }
        }
        else if (arg == "--occlusion-threshold" && i + 1 < argc)
        {
            try
            {
                options.occlusionThreshold = std::stof(argv[++i]);
                if (options.occlusionThreshold < 0.0f || options.occlusionThreshold > 1.0f)
                {
                    std::cerr << "Error: occlusion-threshold must be between 0.0 and 1.0\n";
                    return 1;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: Invalid occlusion-threshold value\n";
                return 1;
            }
        }
        else if (arg[0] != '-')
        {
            if (inputFile.empty())
            {
                inputFile = arg;
            }
            else if (outputFile.empty())
            {
                outputFile = arg;
            }
            else
            {
                std::cerr << "Error: Too many arguments\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        else
        {
            std::cerr << "Error: Unknown option " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate input
    if (inputFile.empty())
    {
        std::cerr << "Error: Input file is required\n";
        printUsage(argv[0]);
        return 1;
    }

    // Check if viewpoints will be available
    if (!options.useExistingCameras && !options.generateViewpoints)
    {
        std::cerr << "Error: At least one viewpoint source must be enabled (cameras or generated)\n";
        return 1;
    }

    // Generate output filename if not provided and not in-place
    if (!inPlace && outputFile.empty())
    {
        size_t dotPos = inputFile.find_last_of('.');
        if (dotPos != std::string::npos)
        {
            outputFile = inputFile.substr(0, dotPos) + "_optimized" + inputFile.substr(dotPos);
        }
        else
        {
            outputFile = inputFile + "_optimized.usd";
        }
    }

    if (options.verbose)
    {
        std::cout << "Hidden Mesh Removal Tool\n";
        std::cout << "========================\n";
        std::cout << "Input file: " << inputFile << "\n";
        if (!dryRun)
        {
            if (inPlace)
            {
                std::cout << "Mode: In-place modification\n";
            }
            else
            {
                std::cout << "Output file: " << outputFile << "\n";
            }
        }
        else
        {
            std::cout << "Mode: Dry run (analysis only)\n";
        }
        std::cout << "Options:\n";
        std::cout << "  Use existing cameras: " << (options.useExistingCameras ? "Yes" : "No") << "\n";
        std::cout << "  Generate viewpoints: " << (options.generateViewpoints ? "Yes" : "No") << "\n";
        std::cout << "  Viewpoint density: " << options.viewpointDensity << "\n";
        std::cout << "  Occlusion threshold: " << options.occlusionThreshold << "\n";
        std::cout << "  Conservative mode: " << (options.conservativeRemoval ? "Yes" : "No") << "\n";
        std::cout << "  Preserve instanced: " << (options.preserveInstancedMeshes ? "Yes" : "No") << "\n";
        std::cout << "\n";
    }

    // Open USD stage
    auto stage = UsdStage::Open(inputFile);
    if (!stage)
    {
        std::cerr << "Error: Could not open USD file: " << inputFile << "\n";
        return 1;
    }

    // Create hidden mesh remover
    workbench::optimizer::HiddenMeshRemover remover(options);

    bool success = false;
    if (dryRun)
    {
        // Analyze without removing
        auto hiddenMeshes = remover.analyzeHiddenMeshes(stage);

        std::cout << "Analysis Results:\n";
        std::cout << "=================\n";
        std::cout << "Hidden meshes found: " << hiddenMeshes.size() << "\n";

        if (options.verbose && !hiddenMeshes.empty())
        {
            std::cout << "\nHidden meshes:\n";
            for (const auto &path : hiddenMeshes)
            {
                std::cout << "  " << path.GetString() << "\n";
            }
        }

        success = true;
    }
    else
    {
        // Remove hidden meshes from the stage
        success = remover.removeHiddenMeshes(stage);

        if (success)
        {
            // Save the result - use Export like triangulate_meshes does
            std::string saveFile = inPlace ? inputFile : outputFile;

            if (options.verbose)
            {
                std::cout << "Saving to: " << saveFile << "\n";
            }

            if (!stage->Export(saveFile))
            {
                std::cerr << "Error: Failed to save USD file: " << saveFile << "\n";
                return 1;
            }

            // Print statistics
            const auto &stats = remover.getStats();
            std::cout << "Hidden Mesh Optimization Completed\n";
            std::cout << "===================================\n";
            std::cout << "Total meshes: " << stats.totalMeshes << "\n";
            std::cout << "Hidden meshes detected: " << stats.hiddenMeshes << "\n";
            std::cout << "Meshes made invisible: " << stats.removedMeshes << "\n";
            std::cout << "Meshes preserved: " << stats.preservedMeshes << "\n";
            std::cout << "Viewpoints used: " << stats.viewpointsUsed << "\n";
            if (stats.viewpointsGenerated > 0)
            {
                std::cout << "Viewpoints generated: " << stats.viewpointsGenerated << "\n";
            }
            std::cout << "Visibility reduction: " << std::fixed << std::setprecision(1)
                      << stats.spaceSavedPercent << "%\n";
        }
    }

    if (!success)
    {
        std::cerr << "Error: Hidden mesh removal failed\n";
        return 1;
    }

    return 0;
}