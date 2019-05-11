#include "utf8/utf8.h"

#include "FileAccessor.h"
#include "paths.h"

using namespace std;
using namespace utf8;

namespace fileaccessor
{
    AbsoluteFilePath::AbsoluteFilePath(const string& _str, bool realFile)
    {
        str = _str;
        m_realFile = realFile;
        normalize();
    }

    string RelativeFileAncestor::location_to_string() const
    {
        switch(locationType)
        {
            case FLT_DOCUMENT: return "[documents]";
            case FLT_LOG: return "[logs]";
            case FLT_BUNDLE: return "[bundle]";
            case FLT_LIBRARY: return "[library]";
            default:
                assert(0);
        }
        return "";
    }

    RelativeFileAncestor::RelativeFileAncestor(const string& path, FileLocationType flt)
    : str(path)
    , locationType(flt)
    {
        normalize();
    }

    RelativeFileAncestor::RelativeFileAncestor(const wstring& path, FileLocationType flt)
    : str(narrow(path))
    , locationType(flt)
    {
        normalize();
    }

    RelativeFileAncestor::RelativeFileAncestor(const wchar_t* path, FileLocationType flt)
    : str(narrow(wstring(path)))
    , locationType(flt)
    {
        normalize();
    }

    RelativeFileAncestor::RelativeFileAncestor(const char* path, FileLocationType flt)
    : str(path)
    , locationType(flt)
    {
        normalize();
    }


    string RelativeFileAncestor::filename() const
    {
        auto found = str.find_last_of("/\\");
        if(found == string::npos)
            return str;
        return str.substr(found + 1);
    }

    string RelativeFileAncestor::path() const
    {
        auto file = filename();
        auto result = str;
        result.erase(str.length() - file.length());
        return result;
    }

    string RelativeFileAncestor::unique_key() const
    {
        return location_to_string() + str;
    }

    string RelativeFileAncestor::basename() const
    {
        auto result = filename();
        auto dot = result.find_last_of(".");
        if(dot != string::npos)
            result.erase(dot);
        return result;
    }

    string RelativeFileAncestor::extension() const
    {
        auto found = str.find_last_of(".");
        if(found == string::npos)
            return "";
        return str.substr(found + 1);
    }


    string RelativeFileBase::to_string() const
    {
        return str;
    }

    wstring RelativeFileBase::to_wstring() const
    {
        return widen(str);
    }


    RelativeFileBase RelativeFileBase::invalid()
    {
        return RelativeFileBase();
    }

    RelativeFilePath RelativeFilePath::invalid()
    {
        return RelativeFilePath();
    }

    string RelativeFilePath::pretty_string() const
    {
        string result = location_to_string();

        result += " " + str + " ";
        auto absPath = FileAccessor::sharedFileAccessor()->resolve(*this);
        result += "(" + absPath.to_string();
        if(!absPath.real_file())
            result += ", zipped file";
        result += ")";
        return result;
    }

    string RelativeFileBase::pretty_string() const
    {
        string result = location_to_string();

        result += " " + str + " ";
        return result;
    }

    wstring RelativeFileAncestor::pretty_wstring() const
    {
        return widen(pretty_string());
    }

    wstring AbsoluteFilePath::to_wstring() const
    {
        return widen(to_string());
    }
}
