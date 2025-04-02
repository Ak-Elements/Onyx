#pragma once

namespace Onyx
{
    // minimum c++20 like span implementation
    template< typename T >
    class Span
    {
    public:
        using Iterator = T*;
        using ConstIterator = const T*;

        constexpr Span()
            : m_Data{ nullptr }
            , m_Size{ 0 }
        {
        }

        constexpr Span(T* data, size_t count)
            : m_Data{ data }
            , m_Size{ count }
        {
        }

        constexpr Span(T* first, T* last)
            : m_Data{ first }
            , m_Size{ std::distance(first, last) }
        {
        }

        constexpr T& operator[](size_t n) const
        {
            return m_Data[n];
        }

        constexpr T* data() const
        {
            return m_Data;
        }

        constexpr onyxU64 size() const
        {
            return m_Size;
        }

        constexpr bool empty() const
        {
            return m_Size == 0;
        }

        constexpr Iterator begin() const
        {
            return Iterator{ data() };
        }

        constexpr Iterator end() const
        {
            return Iterator{ data() + size() };
        }

        constexpr ConstIterator cbegin() const
        {
            return ConstIterator{ begin() };
        }

        constexpr ConstIterator cend() const
        {
            return ConstIterator{ end() };
        }

    private:
        T* m_Data;
        onyxU64 m_Size;
    };
}

