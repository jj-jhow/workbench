#include <algorithm>
#include <string>
#include <cctype>

// Helper to trim whitespace from both ends of a string
inline std::string trim(const std::string &s)
{
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start))
        ++start;
    auto end = s.end();
    do
    {
        --end;
    } while (end >= start && std::isspace(*end));
    return (start <= end) ? std::string(start, end + 1) : std::string();
}
#include "StageManager.h"
#include "converters/ConverterFactory.h"
#include <iostream>
#include <string>
#include <memory>

void print_main_menu()
{
    std::cout << "\nAvailable commands:\n";
    std::cout << "  open_file     - Load a USD file\n";
    std::cout << "  convert_file  - Convert a file (e.g., FBX to USD)\n";
    std::cout << "  help          - Show this menu\n";
    std::cout << "  quit          - Exit the application\n";
}

void print_convert_menu()
{
    std::cout << "\nFile Converter:\n";
    std::cout << "  Enter the path to the input file to convert.\n";
    std::cout << "  Type 'back' to return to the main menu.\n";
    std::cout << "  Type 'help' for options.\n";
}

int main()
{
    StageManager stageManager;
    std::string command;
    bool running = true;

    print_main_menu();
    while (running)
    {
        std::cout << "\n> ";
        std::getline(std::cin, command);
        command = trim(command);

        if (command == "help")
        {
            print_main_menu();
        }
        else if (command == "open_file")
        {
            std::cout << "Enter USD file path: ";
            std::string usdPath;
            std::getline(std::cin, usdPath);
            usdPath = trim(usdPath);
            if (usdPath == "help")
            {
                std::cout << "Type the path to a .usd file to load it, or 'back' to return.\n";
                continue;
            }
            if (usdPath == "back")
                continue;
            if (stageManager.LoadStage(usdPath))
            {
                std::cout << "Successfully loaded USD stage: " << usdPath << std::endl;
            }
            else
            {
                std::cout << "Failed to load USD stage: " << usdPath << std::endl;
            }
        }
        else if (command == "convert_file")
        {
            print_convert_menu();
            std::string inputPath;
            while (true)
            {
                std::cout << "Input file> ";
                std::getline(std::cin, inputPath);
                inputPath = trim(inputPath);
                if (inputPath == "help")
                {
                    print_convert_menu();
                    continue;
                }
                if (inputPath == "back")
                    break;
                std::cout << "Enter desired output format (e.g., usd, fbx): ";
                std::string outputFormat;
                std::getline(std::cin, outputFormat);
                outputFormat = trim(outputFormat);
                if (outputFormat == "help")
                {
                    std::cout << "Type the desired output format (e.g., 'usd' or 'fbx').\n";
                    continue;
                }
                if (outputFormat == "back")
                    break;
                std::cout << "Enter output file path: ";
                std::string outputPath;
                std::getline(std::cin, outputPath);
                outputPath = trim(outputPath);
                if (outputPath == "help")
                {
                    std::cout << "Type the path for the converted file.\n";
                    continue;
                }
                if (outputPath == "back")
                    break;
                auto converter = converters::ConverterFactory::Instance().GetConverterFor(inputPath, outputFormat);
                if (!converter)
                {
                    std::cout << "No converter available for this file type and output format.\n";
                    continue;
                }
                if (converter->Convert(inputPath, outputPath))
                {
                    std::cout << "Conversion succeeded!\n";
                }
                else
                {
                    std::cout << "Conversion failed.\n";
                }
                break;
            }
        }
        else if (command == "quit")
        {
            running = false;
        }
        else if (command.empty())
        {
            continue;
        }
        else
        {
            std::cout << "Unknown command. Type 'help' for options.\n";
        }
    }
    std::cout << "Exiting.\n";
    return 0;
}
