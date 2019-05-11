#include <time.h>
#include <ctime>
#include <stdio.h>
#include <dirent.h>
#include <jni.h>

#include "utf8/utf8.h"

#include "FileAccessor/FileAccessor.h"
#include "FileAccessor/android/io_android.h"
#include "FileAccessor/posix/io_posix.h"
#include "FileAccessor/zip/io_zip.h"
#include "FileAccessor/util/wildcard.h"

using namespace utf8;
using namespace std;
using namespace std::chrono;

extern jmethodID log_java_method_id;
extern jmethodID get_apk_path_method_id;
extern jmethodID get_cache_dir_method_id;
extern jmethodID get_library_dir_method_id;
extern jmethodID get_documents_dir_method_id;
extern jmethodID get_log_dir_method_id;
extern jclass class_fa;
JNIEnv* getJNIEnv();
std::string jstring2string(jstring jstr);

namespace fileaccessor {
    FileReaderAndroid::FileReaderAndroid(const AbsoluteFilePath& zipPath)
    {
        m_posixReader = new FileReaderPOSIX();
        m_zipReader = new FileReaderZip(zipPath);
    }

    shared_ptr<FileBuffer> FileReaderAndroid::getFileBuffer(const RelativeFilePath& path, const FileMode& mode)
    {
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(path);
        if(absPath.real_file())
            return m_posixReader->getFileBuffer(path, mode);
        return m_zipReader->getFileBuffer(path, mode);
    }

    std::shared_ptr<std::istream> FileReaderAndroid::getFileStream(const fileaccessor::RelativeFilePath &file, ios_base::openmode mode)
    {
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(file);
        if(absPath.real_file())
            return m_posixReader->getFileStream(file, mode);
        return m_zipReader->getFileStream(file, mode);
    }

    std::shared_ptr<std::istream> FileReaderAndroid::getFileStream(const fileaccessor::AbsoluteFilePath &file, ios_base::openmode mode)
    {
        return nullptr;
    }

    bool FileReaderAndroid::fileExists(const RelativeFilePath& filename)
    {
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(filename);
        if(absPath.real_file())
            return m_posixReader->fileExists(filename);
        return m_zipReader->fileExists(filename);
    }


    FileEnumeratorAndroid::FileEnumeratorAndroid(const AbsoluteFilePath& zipPath)
    {
        m_zipEnumerator = new FileEnumeratorZip(zipPath);
    }

    RelativeFilePathList FileEnumeratorAndroid::getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime, optional<FileTimePoint> latestCreationTime)
    {
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(path);
        if(absPath.real_file())
        {
            RelativeFilePathList result;
            AbsoluteFilePath fullpath = FileAccessor::sharedFileAccessor()->resolve(path);
            DIR* dp = opendir(fullpath.to_string().c_str());
            if(!dp) return result;

            struct dirent*  ep;

            while ((ep = readdir(dp)) != NULL) {
                if(!strcmp(ep->d_name, ".") ||
                   !strcmp(ep->d_name, ".."))
                    continue;
                string filename(ep->d_name);
                if(wildcard(filemask.begin(), filemask.end(), filename.begin(), filename
                            .end()))
                {
                    result.push_back(path.pathByAppendingString(filename));
                }

            }
            closedir(dp);
            return result;
        }
        return m_zipEnumerator->getFiles(path, filemask, earliestCreationTime, latestCreationTime);
    }

    FileResolverAndroid::FileResolverAndroid(const RelativeFileBase& zipFilePrefix)
    : m_zipFilePrefix(zipFilePrefix)
    , m_cacheEnabled(false)
    {
        JNIEnv *env = getJNIEnv();

        {
            jstring jCacheDir = (jstring)env->CallStaticObjectMethod(class_fa, get_cache_dir_method_id);
            string cacheDir = jstring2string(jCacheDir);
            m_locationCache[FLT_BUNDLE] = constructAbsolutePath(cacheDir);
            env->DeleteLocalRef(jCacheDir);
        }

        {

            jstring jLibraryDir = (jstring)env->CallStaticObjectMethod(class_fa, get_library_dir_method_id);
            string libraryDir = jstring2string(jLibraryDir);
            m_locationCache[FLT_LIBRARY] = constructAbsolutePath(libraryDir);
            env->DeleteLocalRef(jLibraryDir);
        }

        {
            jstring jDocumentDir = (jstring)env->CallStaticObjectMethod(class_fa, get_documents_dir_method_id);
            string documentDir = jstring2string(jDocumentDir);
            m_locationCache[FLT_DOCUMENT] = constructAbsolutePath(documentDir);
            env->DeleteLocalRef(jDocumentDir);
        }

        {
            jstring jLogDir = (jstring)env->CallStaticObjectMethod(class_fa, get_log_dir_method_id);
            string logDir = jstring2string(jLogDir);
            m_locationCache[FLT_LOG] = constructAbsolutePath(logDir);
            env->DeleteLocalRef(jLogDir);
        }
    }

    AbsoluteFilePath FileResolverAndroid::resolve(const RelativeFilePath& relPath)
    {
        if(relPath.getLocationType() != FLT_BUNDLE ||
           m_cacheEnabled)
        {
            return constructAbsolutePath(m_locationCache[relPath.getLocationType()].to_string() + "/" + relPath.plain_string());
        }
        return constructAbsolutePath(m_zipFilePrefix.to_string() + "/" + relPath.plain_string(), false);
    }

    bool FileResolverAndroid::checkCache(const std::string& cacheId)
    {
        FileReaderPOSIX reader;
        auto stream = reader.getFileStream(m_locationCache[FLT_BUNDLE].pathByAppendingString("cache_id.txt"));
        if(!stream || !stream->good())
        {
            return false;
        }

        string temp;
        *stream >> temp;

        if(cacheId != temp)
        {
            return false;
        }
        m_cacheEnabled = true;
        return true;
    }
}
