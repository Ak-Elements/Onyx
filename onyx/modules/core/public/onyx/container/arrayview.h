#pragma once

namespace onyx {

template < typename T, onyxU32 StartIndex, onyxU32 Length > struct ArrayView {
    using Iterator = T*;
    using ConstIterator = const T*;

  public:
    template < typename ContainerOfT > explicit ArrayView( const ContainerOfT& array ) : m_DataView( &array[ m_StartIndex ] ) {
        // static_assert(Length > 0);
        ONYX_ASSERT( array.size() > m_StartIndex );
        ONYX_ASSERT( array.size() > m_EndIndex );
    }

    ArrayView( const ArrayView& ) = delete;
    ArrayView& operator=( const ArrayView& ) = delete;

    ONYX_NO_DISCARD onyxU8 size() const { return Length; }

    const T& operator[]( onyxU8 i ) const {
        // assert size
        ONYX_ASSERT( i < Length );
        return *( *m_DataView )[ m_StartIndex + i ];
    }

    Iterator begin() { return m_DataView[ m_StartIndex ]; }
    ConstIterator begin() const { return m_DataView[ m_StartIndex ]; }
    Iterator end() { return m_DataView[ m_EndIndex ]; }
    ConstIterator end() const { return m_DataView[ m_EndIndex ]; }

  private:
    static constexpr onyxU32 m_StartIndex = StartIndex;
    static constexpr onyxU32 m_EndIndex = StartIndex + ( Length - 1 );

    const T* m_DataView;
};

} // namespace onyx
