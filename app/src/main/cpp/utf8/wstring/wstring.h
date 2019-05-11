#pragma once

#include <string>
#include <sstream>

namespace utf8
{
    void replaceAll(std::wstring& subject, const std::wstring& search,
                    const std::wstring& replace);
    void replaceAll(std::string& subject, const std::string& search,
                    const std::string& replace);
    
	std::wstring wformat(const wchar_t * fmt, ...);
    std::wstring wformat(const wchar_t * fmt, va_list args);
    
    template <typename T>
    std::string NumberToString ( T Number )
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }

    template <typename T>
    T StringToNumber ( const std::string& str )
    {
        T value = 0;
        
        for (int i = 0; i < str.size(); i++)
        {
            value *=  10;
            value += (int) ((str[i]) - '0');
        }
        
        return value;
    }

    template <typename T>
    inline bool ends_with(T const & value, T const & ending)
    {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }
    
    template <typename T>
    inline bool begins_with(T const & value, T const & prefix)
    {
        if (prefix.size() > value.size()) return false;
        return std::equal(prefix.begin(), prefix.end(), value.begin());
    }
    
}
