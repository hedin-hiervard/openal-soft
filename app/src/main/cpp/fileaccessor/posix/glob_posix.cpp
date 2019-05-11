#include <glob.h>
#include <sys/stat.h>
#include <chrono>

#include "utf8/utf8.h"

#include "FileAccessor.h"
#include "GLOB_POSIX.h"

using namespace utf8;
using namespace std;
using namespace std::chrono;

namespace fileaccessor {

    RelativeFilePathList FileEnumeratorPOSIX::getFiles(const RelativeFilePath& path, const std::string& filemask, optional<FileTimePoint> earliestCreationTime, optional<FileTimePoint> latestCreationTime)
    {
        RelativeFilePathList result;
        auto fullpath = FileAccessor::sharedFileAccessor()->resolve(path);

        glob_t g;
        glob((fullpath.to_string() + "/" + filemask).c_str(), GLOB_NOSORT|GLOB_NOESCAPE, NULL, &g);

        auto fullLocation = FileAccessor::sharedFileAccessor()->resolve(RelativeFilePath("", path.getLocationType()));

        for(auto x = 0; x < g.gl_pathc; x++) {
            string temp(g.gl_pathv[x]);
            temp = temp.substr(fullLocation.to_string().length() + 1);
            RelativeFilePath curFileRelPath = RelativeFilePath(temp, path.getLocationType());
            auto curFileAbsPath = FileAccessor::sharedFileAccessor()->resolve(curFileRelPath);

            struct stat attrib {};
            stat(curFileAbsPath.to_string().c_str(), &attrib);
            FileTimePoint then = system_clock::from_time_t(attrib.st_mtime);
            if((!earliestCreationTime.has_value() || (then > earliestCreationTime.value())) &&
               (!latestCreationTime.has_value() || (then < latestCreationTime.value())))
                result.push_back(curFileRelPath);
        }

        globfree(&g);

        return result;
    }
}
