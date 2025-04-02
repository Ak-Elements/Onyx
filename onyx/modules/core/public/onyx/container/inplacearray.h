#pragma once

#include <array>
#include <cctype>

namespace Onyx {

    template <typename T, onyxU8 Size>
    struct InplaceArray
    {
    public:
        using value_type = T;

    private:
       static constexpr onyxU8 MAX_SIZE = Size;
       using iterator = typename std::array<value_type, Size>::iterator;
       using const_iterator = typename std::array<value_type, Size>::const_iterator;

    public:
        InplaceArray() = default;
        explicit InplaceArray(const std::array<value_type, MAX_SIZE>& data)
            : m_Data(data)
            , m_NextIndex(data.size())
        {
        }

        explicit InplaceArray(std::initializer_list<value_type> data)
        {
            ONYX_ASSERT(data.size() <= MAX_SIZE, "Initializer list is too large.");
            for (const value_type& dataElement : data)
            {
                m_Data[m_NextIndex++] = dataElement;
            }
        }

        /*InplaceArray& operator=(InplaceArray other)
        {
            using std::swap;
            swap(*this, other);
            return *this;
        }*/

        void Add(value_type val)
        {
            ONYX_ASSERT(m_NextIndex < MAX_SIZE, "Index out of bounds.");
            m_Data[m_NextIndex++] = val;
        }

        template <typename... Args>
        value_type& Emplace(Args&& ... args)
        {
            return m_Data[m_NextIndex++] = value_type(std::forward<Args>(args)...);
        }

        template <typename... Args>
        value_type& EmplaceAt(onyxU8 i, Args&& ... args)
        {
            ONYX_ASSERT(i < MAX_SIZE, "Index out of bounds.");
            m_NextIndex = i < m_NextIndex ? m_NextIndex : i + 1;
            return m_Data[i] = value_type(std::forward<Args>(args)...);
        }

        bool empty() const { return m_NextIndex == 0; }
        onyxU8 size() const { return m_NextIndex; }
        constexpr onyxU8 capacity() { return MAX_SIZE; }

        value_type& operator[] (onyxU8 i)
        {
            // assert size
            ONYX_ASSERT(i < MAX_SIZE, "Index out of bounds.");
            m_NextIndex = i < m_NextIndex ? m_NextIndex : i + 1;
            return m_Data[i];
        }

        const value_type& operator[] (onyxU8 i) const
        {
            // assert size
            ONYX_ASSERT(i < MAX_SIZE, "Index out of bounds.");
            return m_Data[i];
        }

        void Clear()
        {
            m_NextIndex = 0;
            m_Data = {};
        }

        iterator begin() { return std::begin(m_Data); }
        const_iterator begin() const { return std::begin(m_Data); }
        iterator end() { return begin() + m_NextIndex; }
        const_iterator end() const { return begin() + m_NextIndex; }

        value_type* data() { return m_Data.data(); }
        const value_type* data() const { return m_Data.data(); }

    private:
        std::array<value_type, MAX_SIZE> m_Data{};
        onyxU8 m_NextIndex = 0;
    };

} // namespace Onyx