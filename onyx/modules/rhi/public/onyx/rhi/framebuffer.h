#pragma once

#include <onyx/rhi/graphicstypes.h>

#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/texture.h>

namespace onyx::rhi {
class RenderPass;
class Texture;
struct FramebufferSettings {
  public:
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_LayerCount = 0;
    Reference< RenderPass > m_RenderPass;
    InplaceArray< Reference< Texture, TextureDeleter >, MAX_RENDERPASS_ATTACHMENTS > m_ColorTargets;
    Reference< Texture, TextureDeleter > m_DepthTarget;

    String m_DebugName;

    bool operator==( const FramebufferSettings& other ) const { return Hash() == other.Hash(); }
    uint64_t Hash() const;

  private:
};

class Framebuffer : public RefCounted {
  public:
    Framebuffer() = default;
    Framebuffer( const FramebufferSettings& settings );
    ~Framebuffer() override;

    const FramebufferSettings& GetSettings() { return m_Settings; }
    const FramebufferSettings& GetSettings() const { return m_Settings; }

  protected:
    FramebufferSettings m_Settings;
};
} // namespace onyx::rhi
