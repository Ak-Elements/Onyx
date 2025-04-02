#pragma once

#include <onyx/graphics/bufferproperties.h>

namespace Onyx::Graphics
{
    class Buffer : public RefCounted
    {
    public:
		Buffer(const BufferProperties& properties);

        void* GetData() { return m_DataPointer; }
        virtual void SetData(onyxS32 offset, const void* data, onyxS32 length) = 0;

        const BufferProperties& GetProperties() const { return m_Properties; }

        bool IsValid() const { return m_DataPointer != nullptr; }

		virtual void* Map(MapMode mode) = 0;
		virtual void Unmap() = 0;

		virtual void Flush(onyxU32 offset, onyxU32 count) = 0;

    protected:
		const BufferProperties m_Properties;
		void* m_DataPointer = nullptr;
    };
}