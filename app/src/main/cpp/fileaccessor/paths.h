#pragma once

#include <list>
#include <string>

#include "util/optional.h"

namespace fileaccessor
{
    enum FileLocationType
    {
        FLT_BUNDLE,     // readonly game resources, may be in the zip file
        FLT_LIBRARY,    // writeable path to store hidden from user files
        FLT_DOCUMENT,   // writeable path to store visible to user files
        FLT_LOG,        // store logs (visible to users)
        FLT_COUNT
    };

    class RelativeFileAncestor
    {
        friend class FileAccessor;

    public:
        virtual ~RelativeFileAncestor() {}
        std::string filename() const;
        std::string basename() const;
        std::string path() const;
        std::string extension() const;
        virtual std::string pretty_string() const =0;
        std::wstring pretty_wstring() const;
        std::string unique_key() const;
        std::string location_to_string() const;
        inline bool is_valid() const { return str.length() > 0; }
        inline const FileLocationType& getLocationType() const { return locationType; }
    protected:
        RelativeFileAncestor()
        : locationType(FLT_BUNDLE)
        {}

        RelativeFileAncestor(const std::string& path, FileLocationType flt = FLT_BUNDLE);
        RelativeFileAncestor(const std::wstring& path, FileLocationType flt = FLT_BUNDLE);
        RelativeFileAncestor(const wchar_t* path, FileLocationType flt = FLT_BUNDLE);
        RelativeFileAncestor(const char* path, FileLocationType flt = FLT_BUNDLE);
        void normalize();
    protected:
        std::string str;
        FileLocationType locationType;

    };

    class RelativeFileBase: public RelativeFileAncestor
    {
    public:
        virtual ~RelativeFileBase() {}

        RelativeFileBase(): RelativeFileAncestor() {}
        RelativeFileBase(const std::string& path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        RelativeFileBase(const std::wstring& path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        RelativeFileBase(const wchar_t* path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        RelativeFileBase(const char* path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        inline bool operator==(const RelativeFileBase& other) const
        {
            return str == other.str &&
            locationType == other.locationType;
        }

        inline bool operator!=(const RelativeFileBase& other) const
        {
            return !operator==(other);
        }

        inline bool operator<(const RelativeFileBase& other) const
        {
            return locationType < other.locationType ||
            (locationType == other.locationType &&
             str < other.str);
        }


        std::string to_string() const;
        std::wstring to_wstring() const;
        std::string pretty_string() const;
    public:
        static RelativeFileBase invalid();
    protected:
    };

    class RelativeFilePath: public RelativeFileAncestor
    {
    public:
        virtual ~RelativeFilePath() {}

        explicit RelativeFilePath(): RelativeFileAncestor() {}
        explicit RelativeFilePath(const std::string& path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        explicit RelativeFilePath(const std::wstring& path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        explicit RelativeFilePath(const wchar_t* path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        explicit RelativeFilePath(const char* path, FileLocationType flt = FLT_BUNDLE)
        : RelativeFileAncestor(path, flt)
        {}

        std::string pretty_string() const;

        inline bool operator==(const RelativeFilePath& other) const
        {
            return str == other.str &&
            locationType == other.locationType;
        }

        inline bool operator!=(const RelativeFilePath& other) const
        {
            return !operator==(other);
        }

        RelativeFilePath pathByAppendingString(const std::string& str) const;

        inline const std::string& plain_string() const { return str; }
    public:
        static RelativeFilePath invalid();
    };

    class AbsoluteFilePath
    {
        friend class IFileResolver;
        friend class FileAccessor;
    public:
        inline const std::string& to_string() const { return str; }
        std::wstring to_wstring() const;

        AbsoluteFilePath pathByRemovingLastComponent() const;
        AbsoluteFilePath pathByAppendingString(const std::string& str) const;
        inline bool is_valid() const { return str.length() > 0; }
        inline bool real_file() const { return m_realFile; }
    public:
        static AbsoluteFilePath invalid() { return AbsoluteFilePath(); }
    protected:
        void normalize();
        AbsoluteFilePath(const std::string& _str, bool realFile = true);
        AbsoluteFilePath()
        : m_realFile(false)
        {}
    private:
        std::string str;
        bool m_realFile;
    };

    typedef std::list<RelativeFilePath> RelativeFilePathList;
}
