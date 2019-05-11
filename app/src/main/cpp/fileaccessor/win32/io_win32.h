#pragma once

#include "util/optional.h"

namespace fileaccessor
{
    class FileReaderWin32: public IFileReader
    {
    public:
		std::shared_ptr<FileBuffer> getFileBuffer(const RelativeFilePath& path, const FileMode& mode) override;
		std::shared_ptr<std::istream> getFileStream(const RelativeFilePath& path, std::ios_base::openmode = std::ios_base::in)  override;
        std::shared_ptr<std::istream> getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode = std::ios_base::in)  override;
		bool fileExists(const RelativeFilePath& path) override;
    };

    class FileWriterWin32: public IFileWriter
    {
    public:
        std::shared_ptr<std::ostream> getWritingStream(const RelativeFilePath& path, std::ios_base::openmode mode = std::ios_base::out) override;
        std::shared_ptr<std::ostream> getWritingStream(const AbsoluteFilePath& path, std::ios_base::openmode mode = std::ios_base::out) override;
        void deleteFile(const RelativeFilePath& path) override;
        void createDirectory(const AbsoluteFilePath& path) override;
        void deleteDirectory(const AbsoluteFilePath& path) override;
    };

    class FileEnumeratorWin32: public IFileEnumerator
    {
        RelativeFilePathList getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime = optional<FileTimePoint>::no_value(), optional<FileTimePoint> latestCreationTime = optional<FileTimePoint>::no_value()) override;
    };

    class FileResolverWin32: public IFileResolver
    {
    public:
        FileResolverWin32(const std::string& appName, const std::string& companyName);
        AbsoluteFilePath resolve(const RelativeFilePath& relPath);
        bool checkCache(const std::string& cacheId) { return true; }
    private:
    };

}
