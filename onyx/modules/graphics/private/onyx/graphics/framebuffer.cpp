#include <onyx/graphics/framebuffer.h>

#include <onyx/graphics/texturestorage.h>

namespace Onyx::Graphics
{
    onyxU64 FramebufferSettings::Hash() const
    {
        onyxU64 hash = 0;
        hash = Hash::FNV1aHash<onyxU64>(m_Width, hash);
        hash = Hash::FNV1aHash<onyxU64>(m_Height, hash);
        hash = Hash::FNV1aHash<onyxU64>(&m_LayerCount, hash);

        for (auto colorTarget : m_ColorTargets)
            hash = Hash::FNV1aHash<onyxU64>(colorTarget ? colorTarget.Raw() : 0, hash);

        hash = Hash::FNV1aHash<onyxU64>(m_ColorTargets.size(), hash);
        hash = Hash::FNV1aHash<onyxU64>(m_DepthTarget ? m_DepthTarget.Raw() : 0, hash);
        hash = Hash::FNV1aHash<onyxU64>(m_RenderPass.Raw(), hash);
        // include color and depth target in hash
        //Hash::FNV1aHash64(reinterpret_cast<onyxU8*>(m_DepthTarget), sizeof(onyxU64), hash);

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
