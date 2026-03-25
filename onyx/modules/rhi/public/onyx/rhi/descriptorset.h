#pragma once

namespace onyx::rhi {
struct TextureHandle;
struct BufferHandle;

class DescriptorSet : public RefCounted {
  public:
    DescriptorSet( uint8_t set )
        : m_Set( set ) {}

    virtual bool HasPendingUpdates() const = 0;

    virtual void UpdateDescriptors() = 0;

    virtual void Bind( const TextureHandle& textureHandle, const String& bindingName ) = 0;
    virtual void Bind( const BufferHandle& bufferHandle, const String& bindingName ) = 0;
    virtual HashSet< String > GetBindingIds() const = 0;

    uint8_t GetSet() const { return m_Set; }

  protected:
    int8_t m_Set;
};
} // namespace onyx::rhi
