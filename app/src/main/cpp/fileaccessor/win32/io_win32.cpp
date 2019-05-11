#include <time.h>
#include <ctime>
#include <stdio.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "shlobj.h"
#include <fstream>

#include "utf8/utf8.h"

#include "FileAccessor.h"
#include "IO_WIN32.h"

using namespace utf8;
using namespace std;
using namespace std::chrono;

namespace fileaccessor {
    shared_ptr<FileBuffer> FileReaderWin32::getFileBuffer(const RelativeFilePath& path, const FileMode& mode)
    {
        auto data = make_shared<FileBuffer>();
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
        FILE *fp = _wfopen(fullpath.to_wstring().c_str(), widen(mode).c_str());
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

    std::shared_ptr<std::ostream> FileWriterWin32::getWritingStream(const RelativeFilePath& path, std::ios_base::openmode mode)
    {
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
        return getWritingStream(fullpath);
    }

    std::shared_ptr<std::ostream> FileWriterWin32::getWritingStream(const AbsoluteFilePath& path, std::ios_base::openmode mode)
    {
        auto stream = make_shared<ofstream>();
        stream->open(path.to_wstring().c_str(), mode);
        return stream;
    }


    std::shared_ptr<istream> FileReaderWin32::getFileStream(const RelativeFilePath& path, ios_base::openmode mode)
    {
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
        return getFileStream(fullpath);
    }

    std::shared_ptr<istream> FileReaderWin32::getFileStream(const AbsoluteFilePath& path, ios_base::openmode mode)
    {
        auto stream = make_shared<ifstream>();

        stream->open(path.to_wstring().c_str(), mode);
        return stream;
    }

    void FileWriterWin32::deleteFile(const RelativeFilePath& filename)
    {
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(filename);
        _wremove(fullpath.to_wstring().c_str());
    }

    bool FileReaderWin32::fileExists(const RelativeFilePath& filename)
    {
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(filename);
		return _waccess(fullpath.to_wstring().c_str(), 0) != -1;
    }


    RelativeFilePathList FileEnumeratorWin32::getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime, optional<FileTimePoint> latestCreationTime)
    {
        RelativeFilePathList result;

        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
        auto fullLocation = FileAccessor::sharedFileAccessor()->resolve(RelativeFilePath("", path.getLocationType()));

        WIN32_FIND_DATA fd;
        HANDLE h = FindFirstFile(fullpath.pathByAppendingString(filemask).to_wstring().c_str(), &fd);

        if (h == INVALID_HANDLE_VALUE)
        {
            return result;
        }

        while (1)
        {
            string fullFoundString = fullpath.to_string() + "\\" + narrow(wstring(fd.cFileName));
			RelativeFilePath curFileRelPath = RelativeFilePath(fullFoundString.substr(fullLocation.to_string().length() + 1), path.getLocationType());

            FILETIME ft = fd.ftLastWriteTime;
            SYSTEMTIME sysTime;
            FileTimeToSystemTime(&ft, &sysTime);
            std::tm tm;

            tm.tm_sec = sysTime.wSecond;
            tm.tm_min = sysTime.wMinute;
            tm.tm_hour = sysTime.wHour;
            tm.tm_mday = sysTime.wDay;
            tm.tm_mon = sysTime.wMonth - 1;
            tm.tm_year = sysTime.wYear - 1900;
            tm.tm_isdst = -1;

            FileTimePoint then = system_clock::from_time_t(mktime(&tm));

            if((!earliestCreationTime.has_value() || (then > earliestCreationTime.value())) &&
               (!latestCreationTime.has_value() || (then < latestCreationTime.value())))
                result.push_back(curFileRelPath);
            if (FindNextFile(h, &fd) == FALSE)
                break;
        }

        return result;
    }

    void FileWriterWin32::createDirectory(const fileaccessor::AbsoluteFilePath &path)
    {
        string current = path.to_string();
        auto left = path.pathByRemovingLastComponent();
        if(left.is_valid())
        {
            createDirectory(left);
        }

        wstring wcurrent = widen(current);
        CreateDirectory(wcurrent.c_str(), nullptr);
    }

    void FileWriterWin32::deleteDirectory(const AbsoluteFilePath& path)
    {
        SHFILEOPSTRUCT shfo = {
            NULL,
            FO_DELETE,
            path.to_wstring().c_str(),
            NULL,
            FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
            FALSE,
            NULL,
            NULL };

        SHFileOperation(&shfo);
    }

    FileResolverWin32::FileResolverWin32(const std::string& appName, const std::string& companyName)
    {
        wchar_t szPath[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
		string appDataPath = narrow(wstring(szPath)) + "\\" + companyName + "\\" + appName;

        wchar_t dest[MAX_PATH];
        GetModuleFileName( NULL, dest, sizeof(dest));
        PathRemoveFileSpec(dest);
        string thisPath = narrow(wstring(dest));

        m_locationCache[FLT_BUNDLE] = constructAbsolutePath(thisPath);
        m_locationCache[FLT_LOG] = constructAbsolutePath(appDataPath + "\\log");
        m_locationCache[FLT_DOCUMENT] = constructAbsolutePath(appDataPath + "\\save");
        m_locationCache[FLT_LIBRARY] = constructAbsolutePath(appDataPath);
    }

    AbsoluteFilePath FileResolverWin32::resolve(const RelativeFilePath& path)
    {
        return constructAbsolutePath(m_locationCache[path.getLocationType()].to_string() + "\\" + path.plain_string());
    }
}
