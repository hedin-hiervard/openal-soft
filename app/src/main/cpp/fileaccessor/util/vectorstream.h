#pragma once 

#include <iostream>
#include <streambuf>
#include <memory>
#include <vector>

namespace fileaccessor
{
    template<typename CharT, typename TraitsT = std::char_traits<CharT> >
    class vectorstreambuf : public std::basic_streambuf<CharT, TraitsT> {
    public:
        vectorstreambuf(std::shared_ptr<std::vector<CharT>> vec)
        : m_vec(vec)
        {
            std::basic_streambuf<CharT, TraitsT>::setg(vec->data(), vec->data(), vec->data() + vec->size());
        }
        ~vectorstreambuf()
        {
            
        }
    private:
        std::shared_ptr<std::vector<CharT>> m_vec;
    };
    
    template<typename CharT>
    class vectorstream: public std::basic_istream<CharT>
    {
    public:
        static std::shared_ptr<vectorstream<CharT>> spawn(std::shared_ptr<std::vector<CharT>> vec)
        {
            auto wb = std::make_shared<vectorstreambuf<CharT>>(vec);
            return std::make_shared<vectorstream<CharT>>(wb);
        }

        vectorstream(std::shared_ptr<vectorstreambuf<CharT>> wb)
        :std::basic_istream<CharT>(wb.get())
        , m_streambuf(wb)
        {}
    private:
        std::shared_ptr<vectorstreambuf<CharT>> m_streambuf;
    };
}
