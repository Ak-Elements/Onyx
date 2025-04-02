#pragma once

#include <onyx/graphics/graphicstypes.h>

#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/texture.h>


namespace Onyx::Graphics
{
    class RenderPass;
	class Texture;
    struct FramebufferSettings
    {
    public:
	    onyxU32 m_Width = 0;
	    onyxU32 m_Height = 0;
	    onyxU32 m_LayerCount = 0;
	    Reference<RenderPass> m_RenderPass;
	    InplaceArray<Reference<Texture, TextureDeleter>, MAX_RENDERPASS_ATTACHMENTS> m_ColorTargets;
		Reference<Texture, TextureDeleter> m_DepthTarget;

		String m_DebugName;

	    bool operator==(const FramebufferSettings& other) const { return Hash() == other.Hash(); }

    private:
	    onyxU64 Hash() const;
    };

    class Framebuffer : public RefCounted
    {
    public:
	    Framebuffer() = default;
	    Framebuffer(const FramebufferSettings& settings);
	    ~Framebuffer() override;

	    const FramebufferSettings& GetSettings() { return m_Settings; }
	    const FramebufferSettings& GetSettings() const { return m_Settings; }

    protected:
	    FramebufferSettings m_Settings;
    };
}
