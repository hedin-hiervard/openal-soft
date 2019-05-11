#include <stdio.h>
#include <stdarg.h>
#include "utf8/wstring/wstring.h"

#include <assert.h>
#include <locale.h>

using namespace std;

namespace utf8
{
    void replaceAll(string& subject, const string& search,
                    const string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }

    void replaceAll(wstring& subject, const wstring& search,
                    const wstring& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }
    
#ifndef _WIN32
    // this is a custom version used when the system vswprintf is faulty or unavailable
    int custom_vswprintf( wchar_t* buf, size_t count, const wchar_t* fmt, va_list args )
    {
        const size_t tempbufsize = 256;
        int pnt = 0;
        
        for(size_t x=0;; x++) {
            if(x >= count)
                return -1;
            
            if(fmt[x] == 0) {
                buf[pnt] = 0;
                break;
            } else if(fmt[x] == '%') {
                x++;
                if(fmt[x] == 's') {
                    wchar_t *str = va_arg(args, wchar_t*);
                    size_t y=0;
                    while(str[y] != 0) {
                        //                        assert(pnt < 1000); // this is used to trap forever loops and undefined crashes
                        buf[pnt++] = str[y];
                        y++;
                    }
                } else {
                    char fmtstr[tempbufsize];
                    char str[tempbufsize];
                    fmtstr[0] = '%';
                    size_t n = 1;
                    while(true) {
                        fmtstr[n++] = (char)(fmt[x++]);
                        if(fmt[x-1] == 'd' ||
                           fmt[x-1] == 'X' ||
                           fmt[x-1] == 'x' ||
                           fmt[x-1] == 'c' ||
                           fmt[x-1] == 'f' ||
                           fmt[x-1] == 'u')
                        {
                            fmtstr[n] = 0;
                            x--;
                            break;
                        }
                    }
                    va_list argsCopy;
                    
                    /** The va_list structure is not standardized across all platforms; in particular
                     ** Microsoft seems to have problem with the concept.
                     **/
#if defined( va_copy )
                    va_copy( argsCopy, args );
#elif defined( __va_copy )
                    __va_copy( argsCopy, args );
#else
                    memcpy( &argsCopy, &args, sizeof(va_list) );
#endif
                    
                    vsnprintf(str, sizeof(str), fmtstr, argsCopy);
                    size_t y = 0;
                    while(str[y] != 0) {
                        buf[pnt++] = (wchar_t)(str[y]);
                        y++;
                    }
                    va_arg(args, int);
                }
            } else
                buf[pnt++] = fmt[x];
        }
        return pnt;
    }
#endif
    
    // this is a unicode printf version that runs on all supported platforms
    // it accepts widestring format and widestring arguments
    int internal_wformat(wchar_t * dst, size_t size, const wchar_t *fmt, va_list args)
    {
#if __APPLE__
#include "TargetConditionals.h"
        
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        // NB! iOS's vswprintf doesn't work when argument contains non-ASCII widestring, need to use custom version
        return  custom_vswprintf(dst, size, fmt, args);
#elif TARGET_OS_MAC
        // on OSX we need to replace '%s' with '%ls' and use vswprintf function
        setlocale(LC_CTYPE, "UTF-8");
        wstring tmpfmt = fmt;
        replaceAll(tmpfmt, L"%s", L"%ls");
        return vswprintf(dst, size, tmpfmt.c_str(), args);
#else
#   error "Unknown Apple platform"
#endif
#elif defined(_WIN32)
        // on Windows we just use _vsnwprintf
        return _vsnwprintf(dst, size, fmt, args);
#elif __ANDROID__
        return  custom_vswprintf(dst, size, fmt, args);
#else
        assert(0); // unsupported platform
        return -1;
#endif
    }
    
#define MAX_STRING_SIZE 4096
    
    wstring wformat(const wchar_t * fmt, va_list args)
    {
        wchar_t buffer[MAX_STRING_SIZE];
        int res = internal_wformat(buffer, MAX_STRING_SIZE, fmt, args);
//        assert(res >= 0);
        return wstring(buffer);
    }
    
    wstring wformat(const wchar_t * fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        wstring result = wformat(fmt, args);
        va_end( args );
        return result;
    }

    string format(const char* fmt, ...) {
        size_t size = strlen(fmt) * 2 + 50;   // Use a rubric appropriate for your code
        std::string str;
        va_list ap;
        while (1) {     // Maximum two passes on a POSIX system...
            str.resize(size);
            va_start(ap, fmt);
            int n = vsnprintf((char *)str.data(), size, fmt, ap);
            va_end(ap);
            if (n > -1 && n < (int)size) {  // Everything worked
                str.resize(n);
                return str;
            }
            if (n > -1)  // Needed size returned
                size = n + 1;   // For null char
            else
                size *= 2;      // Guess at a larger size (OS specific)
        }
        return str;
    }

}