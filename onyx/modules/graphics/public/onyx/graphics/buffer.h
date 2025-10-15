#pragma once

#include <onyx/graphics/bufferproperties.h>

namespace Onyx::Graphics
{
    enum class Access : onyxU32;
    enum class Context : onyxU8;
    class CommandBuffer;

    class Buffer : public RefCounted
    {
    public:
		Buffer(const BufferProperties& properties);

        void* GetData() { return m_DataPointer; }
        virtual void SetData(onyxS32 offset, const void* data, onyxS32 length) = 0;

        const BufferProperties& GetProperties() const { return m_Properties; }

        bool IsValid() const { return m_DataPointer != nullptr; }

        onyxU64 GetGpuAddress() const { return m_GpuAddress; }
        virtual onyxU64 GetAliasOffset(onyxS8 alias) const = 0;
        virtual onyxU64 GetAliasSize(onyxS8 alias) const = 0;

		virtual void* Map(MapMode mode) = 0;
		virtual void Unmap() = 0;

		virtual void Flush(onyxU32 offset, onyxU32 count) = 0;

        virtual void Barrier(CommandBuffer& commandBuffer, Context newContext, Access newAccess) = 0;
        virtual void Barrier(CommandBuffer& commandBuffer, Context newContext, Access newAccess, onyxU64 offset, onyxU64 size) = 0;
        virtual onyxS8 Alias(const BufferProperties& properties) = 0;
        virtual void ClearAliases() = 0;

    protected:
		const BufferProperties m_Properties;

        onyxU64 m_GpuAddress = 0;
		void* m_DataPointer = nullptr;
    };
}
