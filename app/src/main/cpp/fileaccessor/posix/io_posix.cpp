#include <fstream>
#include <sys/stat.h>
#include <chrono>
#include <unistd.h>
#include <dirent.h>

#include "utf8/utf8.h"

#include "FileAccessor/FileAccessor.h"
#include "FileAccessor/posix/io_posix.h"

using namespace utf8;
using namespace std;
using namespace std::chrono;

namespace fileaccessor {
    std::shared_ptr<FileBuffer> FileReaderPOSIX::getFileBuffer(const RelativeFilePath& path, const FileMode& mode)
    {
        auto data = make_shared<FileBuffer>();
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);

        FILE *fp = fopen(fullpath.to_string().c_str(), mode.c_str());
        if(!fp)
        {
            return data;
        }

        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        data->resize(size);
        fread(&(*data)[0], sizeof(char), data->size(), fp);
        fclose(fp);

        return data;
    }

    std::shared_ptr<std::istream> FileReaderPOSIX::getFileStream(const fileaccessor::RelativeFilePath &path, ios_base::openmode mode)
    {
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
        return getFileStream(fullpath);
    }

    std::shared_ptr<std::istream> FileReaderPOSIX::getFileStream(const AbsoluteFilePath& path, std::ios_base::openmode mode)
    {
        auto stream = make_shared<ifstream>();

        stream->open(path.to_string().c_str(), mode);
        return stream;
    }

    bool FileReaderPOSIX::fileExists(const RelativeFilePath& path)
    {
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(path);
        struct stat buffer;
        return (stat (absPath.to_string().c_str(), &buffer) == 0);
    }

    std::shared_ptr<std::ostream> FileWriterPOSIX::getWritingStream(const RelativeFilePath& path, ios_base::openmode mode)
    {
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
        return getWritingStream(fullpath);
    }

    std::shared_ptr<std::ostream> FileWriterPOSIX::getWritingStream(const AbsoluteFilePath& path, ios_base::openmode mode)
    {
        auto stream = make_shared<ofstream>();
        stream->open(path.to_string().c_str(), mode);
        if(!stream->good())
        {
            // WARN(_LG("error opening writing stream to %s, the error was: %s"), path.to_wstring().c_str(), widen(strerror(errno)).c_str());
        }
        return stream;
    }

    void FileWriterPOSIX::createDirectory(const fileaccessor::AbsoluteFilePath& path)
    {
        auto left = path.pathByRemovingLastComponent();
        if(left.is_valid())
        {
            createDirectory(left);
        }
        int result = mkdir(path.to_string().c_str(), S_IRWXU);
    }

    void FileWriterPOSIX::deleteDirectory(const AbsoluteFilePath& path)
    {
        DIR*            dp;
        struct dirent*  ep;
        char            p_buf[512] = {0};

        dp = opendir(path.to_string().c_str());
        if(!dp) return;

        while ((ep = readdir(dp)) != NULL) {
            if(!strcmp(ep->d_name, ".") ||
               !strcmp(ep->d_name, ".."))
                continue;

            AbsoluteFilePath newPath = path.pathByAppendingString(ep->d_name);

            struct stat s_buf;

            bool isDir = false;
            if (stat(newPath.to_string().c_str(), &s_buf) == 0)
            {
                isDir = S_ISDIR(s_buf.st_mode);
            }

            if (isDir)
                deleteDirectory(newPath);
            else
                remove(newPath.to_string().c_str());
        }

        closedir(dp);
        rmdir(path.to_string().c_str());
    }

    void FileWriterPOSIX::deleteFile(const RelativeFilePath& path)
    {
        assert(FileAccessor::sharedFileAccessor()->isLocationWriteable(path.getLocationType()));
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(path);
        remove(absPath.to_string().c_str());
    }

}
