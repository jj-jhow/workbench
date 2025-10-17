#pragma once

#include "importers/IImporter.h"

namespace importers
{

    class FbxImporter : public IImporter
    {
    public:
        bool importFile(const std::string &source) override;
    };

} // namespace importers