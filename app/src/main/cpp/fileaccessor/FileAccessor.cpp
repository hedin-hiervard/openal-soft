#include "utf8/utf8.h"

#include "FileAccessor.h"

using namespace utf8;
using namespace std;

namespace fileaccessor
{
    FileAccessor* FileAccessor::sm_instance = nullptr;

    RelativeFilePath FileAccessor::baseToPath(const RelativeFileBase& base)
    {
        return RelativeFilePath(base.to_string(), base.getLocationType());
    }

    void FileAccessor::init(const std::string& appName, const std::string& companyName)
    {
        m_appName = appName;
        m_companyName = companyName;

        platformInit();

        for(auto x = 0; x < FLT_COUNT; x++)
        {
            if(!isLocationWriteable((FileLocationType)x))
                continue;

            RelativeFilePath relPath = RelativeFilePath("", (FileLocationType)x);
            auto absPath = resolve(relPath);
            createDirectory(absPath);
        }
    }
}
