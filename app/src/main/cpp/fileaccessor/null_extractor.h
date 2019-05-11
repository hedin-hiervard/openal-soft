#pragma once

#include <list>
#include <memory>
#include <string>

#include "FileAccessor.h"

namespace fileaccessor
{
    class FileExtractionTaskNull: public IFileExtractionTask
    {
        ~FileExtractionTaskNull() {}
    };

    class FileExtractorNull: public IFileExtractor
    {
    public:
        std::list<std::shared_ptr<IFileExtractionTask>> getExtractionTasks(std::string& cacheId)
        { return std::list<std::shared_ptr<IFileExtractionTask>>(); }
        void executeExtractionTask(std::shared_ptr<IFileExtractionTask> task)
        { }

    };
}
