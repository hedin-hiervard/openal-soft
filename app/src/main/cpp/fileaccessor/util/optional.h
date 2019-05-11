#pragma once

#include <assert.h>

namespace fileaccessor
{
    template<typename T>
    class optional
    {
    public:
        optional(T value)
        : m_value(value)
        , m_hasValue(true)
        {}
        
        optional()
        : m_hasValue(false)
        {}
        
        inline bool has_value() const { return m_hasValue; }
        const T& value() const { assert(m_hasValue); return m_value; }
    public:
        inline static optional no_value() { return optional(); }
    private:
        
    private:
        bool m_hasValue;
        T m_value;
    };
}
