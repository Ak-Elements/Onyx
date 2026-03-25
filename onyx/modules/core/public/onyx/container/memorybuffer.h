#pragma once

#include <onyx/onyx_types.h>

namespace onyx {
template < typename T >
struct MemoryBufferBase {
    using Type = T;
    using PointerT = Type*;

    MemoryBufferBase( uint32_t bufferSize, uint8_t bufferAlignment, T* buffer )
        : m_size( bufferSize )
        , m_alignment( bufferAlignment )
        , m_bufferPtr( buffer ) {}

    virtual ~MemoryBufferBase() {}

    Type* getData() { return m_bufferPtr; }

    Type* operator[]( uint32_t index ) { return m_bufferPtr + index; }

  protected:
    uint32_t m_size;
    uint8_t m_alignment;
    T* m_bufferPtr;
};

template < typename T >
struct MemoryRingBufferBase : public MemoryBufferBase< T > {
    using Base = MemoryBufferBase< T >;
    using typename Base::PointerT;
    using typename Base::Type;

    static constexpr uint64_t ElementSize = sizeof( T );

    MemoryRingBufferBase( uint32_t bufferSize, uint8_t bufferAlignment, T* buffer )
        : Base( bufferSize, bufferAlignment, buffer )
        , m_nextBufferPos( 0 ) {}

    Span< Type > getBuffer() { return getBuffer( Base::m_size ); }

    Span< Type > getBuffer( uint32_t requestedSize ) {
        const uint32_t clampedRequestedSize = std::min< uint32_t >(
            static_cast< uint32_t >( requestedSize * ElementSize ),
            Base::m_size );
        const uint32_t alignedClampedBufferSize = ( clampedRequestedSize + ( Base::m_alignment - 1 ) ) &
                                                  -Base::m_alignment;

        const uint32_t bufferPos = ( m_nextBufferPos + alignedClampedBufferSize ) >= Base::m_size ? 0u
                                                                                                  : m_nextBufferPos;
        m_nextBufferPos = bufferPos + alignedClampedBufferSize;

        const PointerT bufferPtr = reinterpret_cast< PointerT >( Base::m_bufferPtr + bufferPos );
        return { bufferPtr, ( alignedClampedBufferSize / ElementSize ) };
    }

  private:
    uint32_t m_nextBufferPos;
};

template < typename T, uint32_t Size, uint8_t Alignment = alignof( T ) >
struct MemoryBuffer : MemoryBufferBase< T > {
    static_assert( ( Alignment & ( Alignment - 1 ) ) == 0, "Alignment has to be a power of 2!" );
    static_assert( Size % Alignment == 0, "Size is not matching alignment" );

    MemoryBuffer()
        : MemoryBufferBase< T >( Size, Alignment, m_buffer ) {}

  private:
    T m_buffer[ Size ] = { 0 };
};

template < typename T, uint32_t Size, uint8_t Alignment = alignof( T ) >
struct MemoryRingBuffer : MemoryRingBufferBase< T > {
    static_assert( ( Alignment & ( Alignment - 1 ) ) == 0, "Alignment has to be a power of 2!" );
    static_assert( Size % Alignment == 0, "Size is not matching alignment" );

    MemoryRingBuffer()
        : MemoryRingBufferBase< T >( Size, Alignment, m_buffer ) {}

  private:
    T m_buffer[ Size ] = { 0 };
};
} // namespace onyx
