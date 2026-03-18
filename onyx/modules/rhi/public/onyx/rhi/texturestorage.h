#pragma once

#include <onyx/rhi/texturestorageproperties.h>

namespace onyx::rhi
{
    class CommandBuffer;

	// this should be some kind of resource not just ref counted
    class TextureStorage : public RefCounted
    {
    public:
		TextureStorage() = default;
		TextureStorage(const TextureStorageProperties& properties);
        ~TextureStorage() override = default;

		const TextureStorageProperties& GetProperties() const { return m_Properties; }

        virtual void TransitionLayout(CommandBuffer& commandBuffer, Context context, Access access, ImageLayout newLayout) = 0;

    protected:
		TextureStorageProperties m_Properties;
    };
}
