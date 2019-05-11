#pragma once

#include "FileAccessor/FileAccessor.h"

namespace fileaccessor
{
    class FileReaderPOSIX: public IFileReader
    {
    public:
        std::shared_ptr<FileBuffer> getFileBuffer(const RelativeFilePath& path, const FileMode& mode) override;
        std::shared_ptr<std::istream> getFileStream(const RelativeFilePath& path, std::ios_base::openmode = std::ios_base::in)  override;
        std::shared_ptr<std::istream> getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode = std::ios_base::in)  override;
        bool fileExists(const RelativeFilePath& path) override;
    };

    class FileWriterPOSIX: public IFileWriter
    {
    public:
        std::shared_ptr<std::ostream> getWritingStream(const RelativeFilePath& path, std::ios_base::openmode mode = std::ios_base::out) override;
        std::shared_ptr<std::ostream> getWritingStream(const AbsoluteFilePath& path, std::ios_base::openmode mode = std::ios_base::out) override;
        void deleteFile(const RelativeFilePath& path) override;
        void createDirectory(const AbsoluteFilePath& path) override;
        void deleteDirectory(const AbsoluteFilePath& path) override;
    };
}
