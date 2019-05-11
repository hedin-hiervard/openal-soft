#include <jni.h>

#include "utf8/utf8.h"

#include "FileAccessor/FileAccessor.h"
#include "FileAccessor/android/io_android.h"
#include "FileAccessor/posix/io_posix.h"
#include "FileAccessor/zip/io_zip.h"

using namespace std;
using namespace utf8;


extern jmethodID log_java_method_id;
extern jmethodID get_apk_path_method_id;
extern jmethodID get_cache_dir_method_id;
extern jmethodID get_library_dir_method_id;
extern jmethodID get_documents_dir_method_id;
extern jmethodID get_log_dir_method_id;
extern jclass class_fa;
JNIEnv* getJNIEnv();
void log(const std::string& str);

string jstring2string(jstring jstr);

namespace fileaccessor
{
    void FileAccessor::platformInit()
    {
        JNIEnv* env = getJNIEnv();
        jstring jApkPath = (jstring)env->CallStaticObjectMethod(class_fa, get_apk_path_method_id);
        string apkPath = jstring2string(jApkPath);
        env->DeleteLocalRef(jApkPath);

        jstring jCacheDir = (jstring)env->CallStaticObjectMethod(class_fa, get_cache_dir_method_id);
        string cacheDir = jstring2string(jCacheDir);
        m_locationCache[FLT_BUNDLE] = constructAbsolutePath(cacheDir);
        env->DeleteLocalRef(jCacheDir);

        AbsoluteFilePath zipFilePath = constructAbsolutePath(apkPath);
        log("found apk file " + zipFilePath.to_string());

        m_reader = new FileReaderAndroid(zipFilePath);
        m_writer = new FileWriterPOSIX();
        m_enumerator = new FileEnumeratorAndroid(zipFilePath);
        m_resolver = new FileResolverAndroid("assets");
        m_extractor = new FileExtractorZip(zipFilePath, cacheDir, "assets");
    }
}
