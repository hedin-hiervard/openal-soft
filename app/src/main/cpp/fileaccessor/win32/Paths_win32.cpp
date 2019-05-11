#include "Paths.h"

#include "utf8/utf8.h"

using namespace utf8;

namespace fileaccessor
{
    void AbsoluteFilePath::normalize()
    {
        replaceAll(str, "/", "\\");
        replaceAll(str, "\\\\", "\\");

        if(str.length() > 0 && (str.back() == '\\'))
            str.pop_back();
    }

    AbsoluteFilePath AbsoluteFilePath::pathByRemovingLastComponent() const
    {
        auto pos = str.find_last_of("\\");
        if(pos == str.npos)
            return AbsoluteFilePath::invalid();
        return AbsoluteFilePath(str.substr(0, pos));
    }

    AbsoluteFilePath AbsoluteFilePath::pathByAppendingString(const std::string& _str) const
    {
        return AbsoluteFilePath(str + "\\" + _str);
    }


    RelativeFilePath RelativeFilePath::pathByAppendingString(const std::string& _str) const
    {
        return RelativeFilePath(str + "\\" + _str, locationType);
    }

    void RelativeFileAncestor::normalize()
    {
        if(str.length() > 0 && (str.back() == '\\'))
            str.pop_back();

        assert(str.find(':') == str.npos);
    }
}
