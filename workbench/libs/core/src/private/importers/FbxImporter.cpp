#include "importers/FbxImporter.h"
#include <iostream>

namespace importers
{

    bool FbxImporter::importFile(const std::string &source)
    {
        std::cout << "Attempting to import FBX file: " << source << std::endl;
        // For now, we'll just return true to indicate a successful import.
        std::cout << "FBX file imported successfully (mock)." << std::endl;
        return true;
    }

} // namespace importers
