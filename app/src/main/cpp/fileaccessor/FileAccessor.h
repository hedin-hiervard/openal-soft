#pragma once

#include <list>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <vector>

#include "util/optional.h"

#include "paths.h"

namespace fileaccessor
{
    typedef std::chrono::time_point<std::chrono::system_clock> FileTimePoint;
    typedef std::string FileMode;
    typedef std::vector<char> FileBuffer;

    class IFileReader
    {
    public:
        virtual ~IFileReader() {}
        virtual std::shared_ptr<FileBuffer> getFileBuffer(const RelativeFilePath& path, const FileMode& mode) =0;
        virtual std::shared_ptr<std::istream> getFileStream(const RelativeFilePath& path, std::ios_base::openmode = std::ios_base::in) =0;
        virtual std::shared_ptr<std::istream> getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode = std::ios_base::in) =0;
        virtual bool fileExists(const RelativeFilePath& path) =0;
    };

    class IFileWriter
    {
    public:
        virtual ~IFileWriter() {}
        virtual std::shared_ptr<std::ostream> getWritingStream(const RelativeFilePath& path, std::ios_base::openmode = std::ios_base::out) =0;
        virtual std::shared_ptr<std::ostream> getWritingStream(const AbsoluteFilePath& path, std::ios_base::openmode = std::ios_base::out) =0;
        virtual void deleteFile(const RelativeFilePath& path) =0;
        virtual void deleteDirectory(const AbsoluteFilePath& path) =0;
        virtual void createDirectory(const AbsoluteFilePath& path) =0;
    };

    class IFileEnumerator
    {
    public:
        virtual ~IFileEnumerator() {}
        virtual RelativeFilePathList getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime = optional<FileTimePoint>::no_value(), optional<FileTimePoint> latestCreationTime = optional<FileTimePoint>::no_value()) =0;
    };

    class IFileResolver
    {
    public:
        virtual ~IFileResolver() {}
        virtual AbsoluteFilePath resolve(const RelativeFilePath& path) =0;
        virtual bool isLocationWriteable(FileLocationType location)
        { return location != FLT_BUNDLE; }
        virtual bool checkCache(const std::string& cacheId) =0;
    protected:
        AbsoluteFilePath constructAbsolutePath(const std::string& str, bool realFile = true)
        { return AbsoluteFilePath(str, realFile); }
    protected:
        AbsoluteFilePath m_locationCache[FLT_COUNT];
    };

    class IFileExtractionTask
    {
    public:
        virtual ~IFileExtractionTask() {}
    };

    class IFileExtractor
    {
    public:
        virtual ~IFileExtractor() {}
        virtual std::list<std::shared_ptr<IFileExtractionTask>> getExtractionTasks(std::string& cacheId) =0;
        virtual void executeExtractionTask(std::shared_ptr<IFileExtractionTask> task) =0;
    };

    class FileAccessor
    : public IFileReader
    , public IFileWriter
    , public IFileEnumerator
    , public IFileResolver
    , public IFileExtractor
    {
    protected:
        static FileAccessor* sm_instance;
    protected:

        optional<std::string> m_appName;
        optional<std::string> m_companyName;

        IFileReader*        m_reader;
        IFileWriter*        m_writer;
        IFileEnumerator*    m_enumerator;
        IFileResolver*      m_resolver;
        IFileExtractor*     m_extractor;
    protected:
        FileAccessor()
        : m_reader(nullptr)
        , m_writer(nullptr)
        , m_enumerator(nullptr)
        , m_resolver(nullptr)
        , m_extractor(nullptr)
        {}

    public:
        static FileAccessor* sharedFileAccessor()
        {
            if(!sm_instance) sm_instance = new FileAccessor();
            return sm_instance;
        }

        RelativeFilePath baseToPath(const RelativeFileBase& base);

        void init(const std::string& appName, const std::string& companyName);
        void platformInit();
        void runDiagnostics();

        /* reader */
        std::shared_ptr<FileBuffer> getFileBuffer(const RelativeFilePath& path, const FileMode& mode) override
        { return m_reader->getFileBuffer(path, mode); }
        virtual std::shared_ptr<std::istream> getFileStream(const RelativeFilePath& path, std::ios_base::openmode mode = std::ios_base::in) override
        { return m_reader->getFileStream(path, mode); }
        virtual std::shared_ptr<std::istream> getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode mode = std::ios_base::in) override
        { return m_reader->getFileStream(path, mode); }
        bool fileExists(const RelativeFilePath& path) override
        { return m_reader->fileExists(path); }

        /* enumerator */
        RelativeFilePathList getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime = optional<FileTimePoint>::no_value(), optional<FileTimePoint> latestCreationTime = optional<FileTimePoint>::no_value()) override
        { return std::move(m_enumerator->getFiles(path, filemask, earliestCreationTime, latestCreationTime)); }

        /* writer */
        std::shared_ptr<std::ostream> getWritingStream(const RelativeFilePath& path, std::ios_base::openmode mode = std::ios_base::out) override
        { return m_writer->getWritingStream(path, mode); }
        std::shared_ptr<std::ostream> getWritingStream(const AbsoluteFilePath& path, std::ios_base::openmode mode = std::ios_base::out) override
        { return m_writer->getWritingStream(path, mode); }

        void deleteFile(const RelativeFilePath& path) override
        { m_writer->deleteFile(path); }
        void createDirectory(const AbsoluteFilePath& path) override
        { m_writer->createDirectory(path); }
        void deleteDirectory(const AbsoluteFilePath& path) override
        { m_writer->deleteDirectory(path); }

        /* resolver */
        AbsoluteFilePath resolve(const RelativeFilePath& path) override
        { return m_resolver->resolve(path); }
        bool isLocationWriteable(FileLocationType location) override
        { return m_resolver->isLocationWriteable(location); }
        bool checkCache(const std::string& cacheId) override
        { return m_resolver->checkCache(cacheId); }

        /* extractor */
        std::list<std::shared_ptr<IFileExtractionTask>> getExtractionTasks(std::string& cacheId) override
        { return m_extractor->getExtractionTasks(cacheId); }
        void executeExtractionTask(std::shared_ptr<IFileExtractionTask> task) override
        { m_extractor->executeExtractionTask(task); }
    };

}
