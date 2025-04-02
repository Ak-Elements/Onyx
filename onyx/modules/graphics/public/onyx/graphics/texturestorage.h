#pragma once

#include <onyx/graphics/texturestorageproperties.h>

namespace Onyx::Graphics
{
	// this should be some kind of resource not just ref counted
    class TextureStorage : public RefCounted
    {
    public:
		TextureStorage() = default;
		TextureStorage(const TextureStorageProperties& properties);
        ~TextureStorage() override = default;

		const TextureStorageProperties& GetProperties() const { return m_Properties; }

    protected:
		TextureStorageProperties m_Properties;
    };
}
