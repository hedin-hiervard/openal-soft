#pragma once

#include "FileAccessor/util/optional.h"
#include "zipper/unzipper.h"

namespace fileaccessor
{
    class FileReaderZip: public IFileReader
    {
    public:
		std::shared_ptr<FileBuffer> getFileBuffer(const RelativeFilePath& path, const FileMode& mode) override;
		std::shared_ptr<std::istream> getFileStream(const RelativeFilePath& path, std::ios_base::openmode = std::ios_base::in)  override;
        std::shared_ptr<std::istream> getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode = std::ios_base::in) override;
		bool fileExists(const RelativeFilePath& path) override;
    public:
        FileReaderZip(const AbsoluteFilePath& zipFile);
    private:
        std::shared_ptr<zipper::Unzipper> m_unzipper;
    };

    class FileEnumeratorZip: public IFileEnumerator
    {
    public:
        RelativeFilePathList getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime = optional<FileTimePoint>::no_value(), optional<FileTimePoint> latestCreationTime = optional<FileTimePoint>::no_value()) override;
    public:
        FileEnumeratorZip(const AbsoluteFilePath& zipFile);
    private:
        std::shared_ptr<zipper::Unzipper> m_unzipper;
    };

    class FileExtractionTask_DeleteCacheDir: public IFileExtractionTask
    {

    };

    class FileExtractionTask_WriteCacheId: public IFileExtractionTask
    {

    };

    class FileExtractionTask_ExtractFiles: public IFileExtractionTask
    {
    public:
        FileExtractionTask_ExtractFiles()
        {}
    };

    class FileExtractorZip: public IFileExtractor
    {
    public:
        FileExtractorZip(const AbsoluteFilePath& zipPath, const AbsoluteFilePath& cachePath, const std::string& zipPrefix);

        std::list<std::shared_ptr<IFileExtractionTask>> getExtractionTasks(std::string& cacheId);
        void executeExtractionTask(std::shared_ptr<IFileExtractionTask> task);
    private:
        std::shared_ptr<zipper::Unzipper> m_unzipper;
        AbsoluteFilePath m_cachePath;
        std::string m_cacheId;
        std::string m_zipPrefix;
        std::shared_ptr<IFileWriter> m_writer;
    };

}
