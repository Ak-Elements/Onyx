#pragma once
#include <onyx/onyx_types.h>

namespace Onyx
{
    // Very basic inplace string
    template <typename CharT, onyxU32 Size>
    class InplaceStringBase
    {
    public:
        static_assert(Size >= 1);

        InplaceStringBase()
        {
            Clear();
        }

        explicit InplaceStringBase(const CharT* str)
        {
            SetData(str);
        }

        explicit InplaceStringBase(const CharT* str, onyxU32 length)
        {
            SetData(str, length);
        }

        void SetData(const CharT* str)
        {
            if (str == nullptr)
                Clear();
            else
                strcpy_s(&m_Data[0], Size, str);
        }

        void SetData(const CharT* str, onyxU32 length)
        {
            if(str == nullptr)
                Clear();
            else
                SetData(std::basic_string_view<CharT>(str, length));
        }

        void Clear() { m_Data[0] = 0; }

        bool Empty() const { return m_Data[0] == 0; }

        onyxU32 GetLength() const
        {
            return std::char_traits<CharT>::length(m_Data);
        }

        CharT& operator[](onyxU32 index)
        {
            ONYX_ASSERT(index < Size, "Index out of range");
            return m_Data[index];
        }

        const CharT& operator[](onyxU32 index) const
        {
            ONYX_ASSERT(index < Size, "Index out of range");
            return m_Data[index];
        }

        CharT* GetData() { return &m_Data[0]; }
        const CharT* GetData() const { return &m_Data[0]; }

    private:
        CharT m_Data[Size];
    };

    template <onyxU32 Size>
    using InplaceString = InplaceStringBase<char, Size>;
    template <onyxU32 Size>
    using InplaceString_U16 = InplaceStringBase<char16_t, Size>;
    template <onyxU32 Size>
    using InplaceString_U32 = InplaceStringBase<char32_t, Size>;
}
