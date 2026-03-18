#include <onyx/rhi/framebuffer.h>

#include <onyx/rhi/texturestorage.h>

namespace onyx::rhi
{
    onyxU64 FramebufferSettings::Hash() const
    {
        onyxU64 hash = 0;
        hash = hash::FNV1aHash<onyxU64>(m_Width, hash);
        hash = hash::FNV1aHash<onyxU64>(m_Height, hash);
        hash = hash::FNV1aHash<onyxU64>(&m_LayerCount, hash);

        for (auto colorTarget : m_ColorTargets)
            hash = hash::FNV1aHash<onyxU64>(colorTarget ? colorTarget.Raw() : 0, hash);

        hash = hash::FNV1aHash<onyxU64>(m_ColorTargets.size(), hash);
        hash = hash::FNV1aHash<onyxU64>(m_DepthTarget ? m_DepthTarget.Raw() : 0, hash);
        hash = hash::FNV1aHash<onyxU64>(m_RenderPass.Raw(), hash);
        // include color and depth target in hash
        //hash::FNV1aHash64(reinterpret_cast<onyxU8*>(m_DepthTarget), sizeof(onyxU64), hash);

        return hash;
    }

    Framebuffer::Framebuffer(const FramebufferSettings& settings)
        : m_Settings(settings)
    {
    }

    Framebuffer::~Framebuffer()
    {
    }
}
