#pragma once

#include "FileAccessor.h"

namespace fileaccessor
{
    class FileResolveriOS: public IFileResolver
    {
    public:
        FileResolveriOS();
        AbsoluteFilePath resolve(const RelativeFilePath& relPath);
        bool checkCache(const std::string& cacheId) { return true; }
    private:

    };
}
