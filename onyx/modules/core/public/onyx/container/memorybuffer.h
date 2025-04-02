#pragma once

#include <onyx/onyx_types.h>
#include <onyx/container/span.h>

namespace Onyx
{
    template <typename T>
    struct MemoryBufferBase
    {
        using Type = T;
        using PointerT = Type*;

        //static constexpr onyxU64 ElementSize = sizeof(T);

        MemoryBufferBase(onyxU32 bufferSize, onyxU8 bufferAlignment, T* buffer)
            : m_Size(bufferSize)
            , m_Alignment(bufferAlignment)
            , m_BufferPtr(buffer)
        {
        }

        virtual ~MemoryBufferBase()
        {
            
        }

        Type* GetData()
        {
            return m_BufferPtr;
        }

        Type* operator[](onyxU32 index)
        {
            return m_BufferPtr + index;
        }

        /*virtual Span<Type> GetBuffer(onyxU32 requestedSize)
        {
            const onyxU32 clampedRequestedSize = std::min<onyxU32>(static_cast<onyxU32>(requestedSize * ElementSize), m_Size);
            const onyxU32 alignedClampedBufferSize = (clampedRequestedSize + (m_Alignment - 1)) & -m_Alignment;

            const PointerT bufferPtr = reinterpret_cast<PointerT>(m_BufferPtr);
            return { bufferPtr, (alignedClampedBufferSize / ElementSize) };
        }*/

    protected:
        onyxU32 m_Size;
        onyxU8 m_Alignment;
        T* m_BufferPtr;
    };

    template <typename T>
    struct MemoryRingBufferBase : public MemoryBufferBase<T>
    {
        using Base = MemoryBufferBase<T>;
        using typename Base::Type;
        using typename Base::PointerT;

        static constexpr onyxU64 ElementSize = sizeof(T);

        MemoryRingBufferBase(onyxU32 bufferSize, onyxU8 bufferAlignment, T* buffer)
            : Base(bufferSize, bufferAlignment, buffer)
            , m_NextBufferPos(0)
        {
        }

        Span<Type> GetBuffer()
        {
            return GetBuffer(Base::m_Size);
        }

        Span<Type> GetBuffer(onyxU32 requestedSize)
        {
            const onyxU32 clampedRequestedSize = std::min<onyxU32>(static_cast<onyxU32>(requestedSize * ElementSize), Base::m_Size);
            const onyxU32 alignedClampedBufferSize = (clampedRequestedSize + (Base::m_Alignment - 1)) & -Base::m_Alignment;

            const onyxU32 bufferPos = (m_NextBufferPos + alignedClampedBufferSize) >= Base::m_Size ? 0u : m_NextBufferPos;
            m_NextBufferPos = bufferPos + alignedClampedBufferSize;

            const PointerT bufferPtr = reinterpret_cast<PointerT>(Base::m_BufferPtr + bufferPos);
            return { bufferPtr, (alignedClampedBufferSize / ElementSize) };
        }

    private:
        onyxU32 m_NextBufferPos;
    };

    template <typename T, onyxU32 Size, onyxU8 Alignment = alignof(T)>
    struct MemoryBuffer : MemoryBufferBase<T>
    {
        static_assert((Alignment& (Alignment - 1)) == 0, "Alignment has to be a power of 2!");
        static_assert(Size% Alignment == 0, "Size is not matching alignment");

        MemoryBuffer() : MemoryBufferBase<T>(Size, Alignment, m_Buffer) {}

    private:
        T m_Buffer[Size] = { 0 };
    };

    template <typename T, onyxU32 Size, onyxU8 Alignment = alignof(T)>
    struct MemoryRingBuffer : MemoryRingBufferBase<T>
    {
        static_assert((Alignment& (Alignment - 1)) == 0, "Alignment has to be a power of 2!");
        static_assert(Size% Alignment == 0, "Size is not matching alignment");

        MemoryRingBuffer() : MemoryRingBufferBase<T>(Size, Alignment, m_Buffer) {}

    private:
        T m_Buffer[Size] = { 0 };
    };
}
