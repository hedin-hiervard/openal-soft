#pragma once

#include "FileAccessor.h"

namespace fileaccessor
{
    class FileEnumeratorPOSIX: public IFileEnumerator
    {
    public:
        RelativeFilePathList getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime = optional<FileTimePoint>::no_value(), optional<FileTimePoint> latestCreationTime = optional<FileTimePoint>::no_value()) override;
    };
}
