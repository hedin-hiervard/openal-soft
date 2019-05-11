#import <Foundation/Foundation.h>

#include "utf8/utf8.h"

#include "FileAccessor.h"
#include "IO_IOS.h"

using namespace utf8;
using namespace std;

namespace fileaccessor {
    FileResolveriOS::FileResolveriOS()
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* appSupport = [paths objectAtIndex: 0];

        NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
        m_locationCache[FLT_BUNDLE] = constructAbsolutePath(string([resourcePath UTF8String]));

        NSArray* libraryPaths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
        NSString *libraryDir = [paths objectAtIndex:0];
        m_locationCache[FLT_LIBRARY] = constructAbsolutePath(string([libraryDir UTF8String]));
        m_locationCache[FLT_LOG] = constructAbsolutePath(string([libraryDir UTF8String]) + "/log");

        NSArray *documentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex:0];
        m_locationCache[FLT_DOCUMENT] = constructAbsolutePath(string([documentsDir UTF8String]));
    }

    AbsoluteFilePath FileResolveriOS::resolve(const RelativeFilePath& path)
    {
        return constructAbsolutePath(m_locationCache[path.getLocationType()].to_string() + "/" + path.plain_string());
    }
}
