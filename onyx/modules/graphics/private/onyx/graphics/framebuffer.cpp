#include <onyx/graphics/framebuffer.h>

#include <onyx/graphics/texturestorage.h>

namespace Onyx::Graphics
{
    onyxU64 FramebufferSettings::Hash() const
    {
        onyxU64 hash = 0;
        Hash::FNV1aHash64(reinterpret_cast<const onyxU8*>(&m_Width), sizeof(onyxU32), hash);
        Hash::FNV1aHash64(reinterpret_cast<const onyxU8*>(&m_Height), sizeof(onyxU32), hash);
        Hash::FNV1aHash64(reinterpret_cast<const onyxU8*>(&m_LayerCount), sizeof(onyxU32), hash);
        Hash::FNV1aHash64(reinterpret_cast<onyxU8*>(m_ColorTargets.size()), sizeof(onyxU64), hash);
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
