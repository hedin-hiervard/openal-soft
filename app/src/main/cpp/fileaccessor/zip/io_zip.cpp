#include <time.h>
#include <ctime>
#include <stdio.h>
#include <iostream>

#include "utf8/utf8.h"

#include "FileAccessor/FileAccessor.h"
#include "FileAccessor/zip/io_zip.h"
#include "FileAccessor/posix/io_posix.h"
#include "FileAccessor/util/vectorstream.h"
#include "FileAccessor/util/wildcard.h"

using namespace utf8;
using namespace std;
using namespace std::chrono;
using namespace zipper;

namespace fileaccessor {
    FileReaderZip::FileReaderZip(const AbsoluteFilePath& zipFile)
    {
        m_unzipper = make_shared<Unzipper>(zipFile.to_string());
    }

    shared_ptr<FileBuffer> FileReaderZip::getFileBuffer(const RelativeFilePath& path, const FileMode& mode)
    {
        auto data = make_shared<FileBuffer>();
        if(m_unzipper->extractEntryToMemory("assets/" + path.plain_string(), *data)) {
            return data;
        }
        return nullptr;
    }


    std::shared_ptr<std::istream> FileReaderZip::getFileStream(const RelativeFilePath& path, ios_base::openmode mode)
    {
        auto buffer = getFileBuffer(path, "r");
        auto stream = vectorstream<char>::spawn(buffer);
        return stream;
    }

    std::shared_ptr<std::istream> FileReaderZip::getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode)
    {
        return nullptr;
    }

    bool FileReaderZip::fileExists(const RelativeFilePath& filename)
    {
        return m_unzipper->entryExists("assets/" + filename.plain_string());
    }

    FileEnumeratorZip::FileEnumeratorZip(const AbsoluteFilePath& zipFile)
    {
        m_unzipper = make_shared<Unzipper>(zipFile.to_string());
    }

    RelativeFilePathList FileEnumeratorZip::getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime, optional<FileTimePoint> latestCreationTime)
    {
        assert(!earliestCreationTime.has_value() && !latestCreationTime.has_value());
        RelativeFilePathList result;
        string searchPath = "assets/" + path.plain_string();
        string plainRelPath = path.plain_string();
        auto location = path.getLocationType();
        m_unzipper->iterateEntries([&result, &plainRelPath, &searchPath, &filemask, &location](ZipEntry& entry)
                                   {
                                       if(!begins_with(entry.name, searchPath))
                                           return;
                                       string left = entry.name.substr(searchPath.length() + 1);
                                       if(!wildcard(filemask.begin(), filemask.end(), left.begin(), left.end()))
                                           return;

                                       //            FileTimePoint then = system_clock::from_time_t(entry.unixdate.);
                                       result.push_back(RelativeFilePath(plainRelPath + "/" + left, location));
                                   });
        return result;
    }

    FileExtractorZip::FileExtractorZip(const AbsoluteFilePath& zipPath, const AbsoluteFilePath& cachePath, const std::string& zipPrefix)
    : m_cachePath(cachePath)
    , m_zipPrefix(zipPrefix)
    {
        m_unzipper = make_shared<Unzipper>(zipPath.to_string());
        m_writer = make_shared<FileWriterPOSIX>();
    }

    std::list<std::shared_ptr<IFileExtractionTask>> FileExtractorZip::getExtractionTasks(std::string& cacheId)
    {
        m_cacheId = cacheId;
        std::list<std::shared_ptr<IFileExtractionTask>> result;
        result.push_back(make_shared<FileExtractionTask_DeleteCacheDir>());
        result.push_back(make_shared<FileExtractionTask_ExtractFiles>());
        result.push_back(make_shared<FileExtractionTask_WriteCacheId>());
        return result;
    }

    void FileExtractorZip::executeExtractionTask(std::shared_ptr<IFileExtractionTask> task)
    {
        if(auto deleteDirTask = dynamic_pointer_cast<FileExtractionTask_DeleteCacheDir>(task))
        {
            m_writer->deleteDirectory(m_cachePath);
        } else if(auto unzipTask = dynamic_pointer_cast<FileExtractionTask_ExtractFiles>(task))
        {
            if(m_unzipper->extract([this](ZipEntry& entry) -> shared_ptr<ostream>
                                   {
                                       if(!begins_with(entry.name, m_zipPrefix))
                                           return nullptr;
                                       string left = entry.name.substr(m_zipPrefix.length() + 1);
                                       AbsoluteFilePath extractionPath = m_cachePath.pathByAppendingString(left);
                                       // INFO(_LG("extracting to %s"), extractionPath.to_wstring().c_str());
                                       m_writer->createDirectory(extractionPath.pathByRemovingLastComponent());
                                       return m_writer->getWritingStream(extractionPath);
                                   }))
            {
                // INFO(_LG("finished extracting"));
            }
            else
            {
                // INFO(_LG("failed extracting"));
            }
        } else if(auto writeCacheTask = dynamic_pointer_cast<FileExtractionTask_WriteCacheId>(task))
        {
            // INFO(_LG("writing cache id"));
            auto stream = m_writer->getWritingStream(m_cachePath.pathByAppendingString("cache_id.txt"));
            *stream << m_cacheId;
        }
    }
}
