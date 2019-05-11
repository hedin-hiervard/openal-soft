#pragma once

#include "FileAccessor.h"

namespace fileaccessor
{
    class FileResolverMac: public IFileResolver
    {
    public:
        FileResolverMac(const std::string& appName);
        AbsoluteFilePath resolve(const RelativeFilePath& relPath);
        bool checkCache(const std::string& cacheId) { return true; }
    private:
        std::string m_appName;
    };
}
