#pragma once

#include <onyx/rhi/texturestorageproperties.h>

namespace onyx::rhi {
class CommandBuffer;

// this should be some kind of resource not just ref counted
class TextureStorage : public RefCounted {
  public:
    TextureStorage() = default;
    TextureStorage( const TextureStorageProperties& properties );
    ~TextureStorage() override = default;

    [[nodiscard]] const TextureStorageProperties& getProperties() const { return m_properties; }

    virtual void transitionLayout( CommandBuffer& commandBuffer,
                                   Context context,
                                   Access access,
                                   ImageLayout newLayout ) = 0;

  protected:
    TextureStorageProperties m_properties;
};
} // namespace onyx::rhi
