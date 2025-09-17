#pragma once

#include <string>

namespace importers
{

    class IImporter
    {
    public:
        virtual ~IImporter() = default;
        virtual bool importFile(const std::string &source) = 0;
    };

} // namespace importers
