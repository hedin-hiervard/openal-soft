#import <Foundation/Foundation.h>

#include "utf8/utf8.h"

#include "FileAccessor.h"
#include "IO_MAC.h"

using namespace utf8;
using namespace std;

namespace fileaccessor
{
    FileResolverMac::FileResolverMac(const string& appName)
    : m_appName(appName)
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* appSupport = [paths objectAtIndex: 0];
        string appSupportPath =  string([appSupport UTF8String]) + "/" + m_appName;

        NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
        m_locationCache[FLT_BUNDLE] = constructAbsolutePath(string([resourcePath UTF8String]));
        m_locationCache[FLT_LOG] = constructAbsolutePath(appSupportPath + "/log");
        m_locationCache[FLT_DOCUMENT] = constructAbsolutePath(appSupportPath + "/save");
        m_locationCache[FLT_LIBRARY] = constructAbsolutePath(appSupportPath + "/");
    }

    AbsoluteFilePath FileResolverMac::resolve(const RelativeFilePath& path)
    {
        return constructAbsolutePath(m_locationCache[path.getLocationType()].to_string() + "/" + path.plain_string());
    }
}
