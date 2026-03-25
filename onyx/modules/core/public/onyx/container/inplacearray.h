#pragma once

#include <array>

namespace onyx {

// TODO: Can be fully replaced by std::array or std::inplace_vector (c++26)
template < typename T, uint8_t Size >
struct InplaceArray {
  public:
    using ValueType = T;

  private:
    static constexpr uint8_t MaxSize = Size;
    using Iterator = typename std::array< ValueType, Size >::iterator;
    using ConstIterator = typename std::array< ValueType, Size >::const_iterator;

  public:
    InplaceArray() = default;
    explicit InplaceArray( const std::array< ValueType, MaxSize >& data )
        : m_data( data )
        , m_nextIndex( data.size() ) {}

    /*explicit InplaceArray(std::initializer_list<value_type> data)
    {
        ONYX_ASSERT(data.size() <= MAX_SIZE, "Initializer list is too large.");
        for (const value_type& dataElement : data)
        {
            m_Data[m_NextIndex++] = dataElement;
        }
    }*/

    constexpr InplaceArray( std::initializer_list< ValueType > data ) {
        ONYX_ASSERT( data.size() <= MaxSize, "Initializer list is too large." );
        for ( const ValueType& dataElement : data ) {
            m_data[ m_nextIndex++ ] = dataElement;
        }
    }

    /*InplaceArray& operator=(InplaceArray other)
    {
        using std::swap;
        swap(*this, other);
        return *this;
    }*/

    void add( ValueType val ) {
        ONYX_ASSERT( m_nextIndex < MaxSize, "Index out of bounds." );
        m_data[ m_nextIndex++ ] = val;
    }

    template < typename... Args >
    ValueType& emplace( Args&&... args ) {
        return m_data[ m_nextIndex++ ] = ValueType( std::forward< Args >( args )... );
    }

    template < typename... Args >
    ValueType& emplaceAt( uint8_t i, Args&&... args ) {
        ONYX_ASSERT( i < MaxSize, "Index out of bounds." );
        m_nextIndex = i < m_nextIndex ? m_nextIndex : i + 1;
        return m_data[ i ] = ValueType( std::forward< Args >( args )... );
    }

    ONYX_NO_DISCARD constexpr bool empty() const { return m_nextIndex == 0; }
    ONYX_NO_DISCARD constexpr uint8_t size() const { return m_nextIndex; }
    ONYX_NO_DISCARD constexpr uint8_t capacity() { return MaxSize; }

    constexpr ValueType& operator[]( uint8_t i ) {
        // assert size
        ONYX_ASSERT( i < MaxSize, "Index out of bounds." );
        m_nextIndex = i < m_nextIndex ? m_nextIndex : i + 1;
        return m_data[ i ];
    }

    constexpr const ValueType& operator[]( uint8_t i ) const {
        // assert size
        ONYX_ASSERT( i < MaxSize, "Index out of bounds." );
        return m_data[ i ];
    }

    void clear() {
        m_nextIndex = 0;
        m_data = {};
    }

    Iterator begin() { return std::begin( m_data ); }
    ConstIterator begin() const { return std::begin( m_data ); }
    Iterator end() { return begin() + m_nextIndex; }
    ConstIterator end() const { return begin() + m_nextIndex; }

    ValueType* data() { return m_data.data(); }
    const ValueType* data() const { return m_data.data(); }

  private:
    std::array< ValueType, MaxSize > m_data{};
    uint8_t m_nextIndex = 0;
};

} // namespace onyx