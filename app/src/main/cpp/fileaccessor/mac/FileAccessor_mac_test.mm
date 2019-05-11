#import <Foundation/Foundation.h>

#include "IO_POSIX.h"
#include "IO_MAC.h"
#include "IO_ZIP.h"
#include "GLOB_POSIX.h"

#include "logog/include/logog.hpp"

#include "utf8/utf8.h"

using namespace std;
using namespace std::chrono;
using namespace utf8;

#undef LOGOG_GROUP
#define LOGOG_GROUP "fileaccessor"

namespace fileaccessor
{

    class FileResolverMacTest: public IFileResolver
    {
    public:
        bool checkCache(const std::string& cacheId)
        {
            FileReaderPOSIX reader;
            INFO(_LG("checking file cache"));
            auto stream = reader.getFileStream(m_cacheIdPath);
            if(!stream || !stream->good())
            {
                INFO(_LG("cache id not found"));
                return false;
            }

            string temp;
            *stream >> temp;

            if(cacheId != temp)
            {
                WARN(_LG("cache id found but not valid: %s vs %s"), widen(temp).c_str(), widen(cacheId).c_str());
                return false;
            }
            INFO(_LG("valid cache found, activating cache"));
            m_cacheEnabled = true;
            return true;
        }


        AbsoluteFilePath resolve(const RelativeFilePath& relPath)
        {
            if(relPath.getLocationType() != FLT_BUNDLE ||
               m_cacheEnabled)
            {
                return constructAbsolutePath(m_locationCache[relPath.getLocationType()].to_string() + "/" + relPath.plain_string());
            }
            return constructAbsolutePath(m_zipFilePrefix.to_string() + "/" + relPath.plain_string(), false);
        }

        FileResolverMacTest(const RelativeFileBase& zipFilePrefix, const AbsoluteFilePath& cacheIdPath)
        : m_zipFilePrefix(zipFilePrefix)
        , m_cacheEnabled(false)
        , m_cacheIdPath(cacheIdPath)
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
            NSString* appSupport = [paths objectAtIndex: 0];
            string appSupportPath =  string([appSupport UTF8String]) + "/" + "Royal Bounty HD";

            NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
            m_locationCache[FLT_BUNDLE] = constructAbsolutePath(string([resourcePath UTF8String]));
            m_locationCache[FLT_LOG] = constructAbsolutePath(appSupportPath + "/log");
            m_locationCache[FLT_DOCUMENT] = constructAbsolutePath(appSupportPath + "/save");
            m_locationCache[FLT_LIBRARY] = constructAbsolutePath(appSupportPath + "/");
        }
    private:
        RelativeFileBase m_zipFilePrefix;
        AbsoluteFilePath m_cacheIdPath;
        bool m_cacheEnabled;
    };


    void FileAccessor::platformInit()
    {
        AbsoluteFilePath zipPath("/Users/hedin/Dropbox/iosoftware-general/builds-android/RBHD/RBHD-android-gplay-build320.apk");
        AbsoluteFilePath cachePath("/Users/hedin/android-cache");

        m_reader = new FileReaderZip(zipPath);
        m_writer = new FileWriterPOSIX();
        m_enumerator = new FileEnumeratorPOSIX();
        m_resolver = new FileResolverMacTest("assets", AbsoluteFilePath("/Users/hedin/android-cache/cache_id.txt"));
        m_extractor = new FileExtractorZip(zipPath, cachePath, "assets");
    }
}
