#pragma once

namespace Onyx {

    template <typename T, onyxU32 StartIndex, onyxU32 Length>
    struct ArrayView
    {
        using iterator = T*;
        using const_iterator = const T*;

    public:
        template <typename ContainerOfT>
        explicit ArrayView(const ContainerOfT& array)
            : m_DataView(&array[m_StartIndex])
        {
            //static_assert(Length > 0);
            ONYX_ASSERT(array.size() > m_StartIndex);
            ONYX_ASSERT(array.size() > m_EndIndex);
        }

        ArrayView(const ArrayView&) = delete;
        ArrayView& operator= (const ArrayView&) = delete;

        onyxU8 size() const { return Length; }

        const T& operator[] (onyxU8 i) const
        {
            // assert size
            ONYX_ASSERT(i < Length);
            return *(*m_DataView)[m_StartIndex + i];
        }

        iterator begin() { return m_DataView[m_StartIndex]; }
        const_iterator begin() const { return m_DataView[m_StartIndex]; }
        iterator end() { return m_DataView[m_EndIndex]; }
        const_iterator end() const { return m_DataView[m_EndIndex]; }

    private:
        static constexpr onyxU32 m_StartIndex = StartIndex;
        static constexpr onyxU32 m_EndIndex = StartIndex + (Length - 1);

        const T* m_DataView;
    };

} // namespace Onyx
