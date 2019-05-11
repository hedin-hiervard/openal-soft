#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

namespace fileaccessor
{
    template<class Pattern, class Text>
    bool wildcard(
                  Pattern const pat_begin, Pattern const pat_end,
                  Text text_begin, Text const text_end)
    {
        ptrdiff_t const pat_size = pat_end - pat_begin;
        ptrdiff_t stackbuf[64];
        size_t c = sizeof(stackbuf) / sizeof(*stackbuf);
        ptrdiff_t *p = stackbuf;
        size_t n = 0;
        p[n++] = 0;
        while (n > 0 && text_begin != text_end)
        {
            for (size_t i = 0; i < n; i++)
            {
                if (p[i] == pat_size)
                {
                    p[i--] = p[--n];
                    continue;
                }
                switch (*(pat_begin + p[i]))
                {
                    case '?': ++p[i]; break;
                    case '*':
                        ptrdiff_t off;
                        off = p[i];
                        while (off < pat_size &&
                               *(pat_begin + off) == '*')
                        { ++off; }
                        if (n == c)
                        {
                            ptrdiff_t const *const old = p;
                            c *= 2;
                            if (c == 0) { ++c; }
                            size_t const size = c * sizeof(*p);
                            p = (ptrdiff_t *)realloc(
                                                     old == stackbuf ? NULL : p,
                                                     size);
                            if (old == stackbuf)
                            { memcpy(p, old, n * sizeof(*old)); }
                        }
                        p[n++] = off;
                        break;
                    default:
                        if (*(pat_begin + p[i]) == *text_begin)
                        { ++p[i]; }
                        else { p[i--] = p[--n]; }
                        break;
                }
            }
            ++text_begin;
        }
        bool success = false;
        if (text_begin == text_end)
        {
            while (!success && n > 0)
            {
                --n;
                while (p[n] != pat_size &&
                       *(pat_begin + p[n]) == '*')
                { ++p[n]; }
                if (p[n] == pat_size)
                { success = true; }
            }
        }
        if (p != stackbuf) { free(p); }
        return success;
    }
}
