#pragma once

#include "FileAccessor/util/optional.h"

namespace fileaccessor
{
    class FileReaderAndroid: public IFileReader
    {
    public:
		std::shared_ptr<FileBuffer> getFileBuffer(const RelativeFilePath& path, const FileMode& mode) override;
        std::shared_ptr<std::istream> getFileStream(const fileaccessor::RelativeFilePath &path, std::ios_base::openmode mode) override;
        std::shared_ptr<std::istream> getFileStream(const fileaccessor::AbsoluteFilePath &path, std::ios_base::openmode mode) override;
		bool fileExists(const RelativeFilePath& path) override;
    public:
        FileReaderAndroid(const AbsoluteFilePath& zipPath);
    private:
        IFileReader* m_posixReader;
        IFileReader* m_zipReader;
    };

    class FileEnumeratorAndroid: public IFileEnumerator
    {
    public:
        FileEnumeratorAndroid(const AbsoluteFilePath& zipPath);

        RelativeFilePathList getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime = optional<FileTimePoint>::no_value(), optional<FileTimePoint> latestCreationTime = optional<FileTimePoint>::no_value()) override;
    private:
        IFileEnumerator* m_zipEnumerator;
    };

    class FileResolverAndroid: public IFileResolver
    {
    public:
        FileResolverAndroid(const RelativeFileBase& zipFilePrefix);
        AbsoluteFilePath resolve(const RelativeFilePath& relPath);
        bool checkCache(const std::string& cacheId);
    private:
        RelativeFileBase m_zipFilePrefix;
        bool m_cacheEnabled;
    };
}
