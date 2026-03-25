#pragma once
#include <onyx/onyx_types.h>

namespace onyx {
// Very basic inplace string
template < typename CharT, uint32_t Size >
class InplaceStringBase {
  public:
    static_assert( Size >= 1 );

    InplaceStringBase() { clear(); }

    explicit InplaceStringBase( const CharT* str ) { setData( str ); }

    explicit InplaceStringBase( const CharT* str, uint32_t length ) { setData( str, length ); }

    void setData( const CharT* str ) {
        if ( str == nullptr )
            clear();
        else
            strcpy_s( &m_data[ 0 ], Size, str );
    }

    void setData( const CharT* str, uint32_t length ) {
        if ( str == nullptr )
            clear();
        else
            setData( std::basic_string_view< CharT >( str, length ) );
    }

    void clear() { m_data[ 0 ] = 0; }

    ONYX_NO_DISCARD bool empty() const { return m_data[ 0 ] == 0; }

    ONYX_NO_DISCARD uint32_t getLength() const { return std::char_traits< CharT >::length( m_data ); }

    CharT& operator[]( uint32_t index ) {
        // ONYX_ASSERT(index < Size, "Index out of range");
        return m_data[ index ];
    }

    const CharT& operator[]( uint32_t index ) const {
        // ONYX_ASSERT(index < Size, "Index out of range");
        return m_data[ index ];
    }

    CharT* getData() { return &m_data[ 0 ]; }
    const CharT* getData() const { return &m_data[ 0 ]; }

  private:
    CharT m_data[ Size ];
};

template < uint32_t Size >
using InplaceString = InplaceStringBase< char, Size >;
template < uint32_t Size >
using InplaceStringU16 = InplaceStringBase< char16_t, Size >;
template < uint32_t Size >
using InplaceStringU32 = InplaceStringBase< char32_t, Size >;
} // namespace onyx
