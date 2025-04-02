#include <onyx/graphics/framebuffercache.h>

#include <onyx/hash.h>
#include <onyx/graphics/framebuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/vulkan/graphicsapi.h>

namespace Onyx::Graphics
{
    FramebufferCache::FramebufferCache(GraphicsApi& graphicsApi)
        : m_GraphicsApi(graphicsApi)
    {
    }

    void FramebufferCache::Clear()
    {
        m_Cache.clear();
    }

    FramebufferHandle FramebufferCache::GetOrCreateFramebuffer(const FramebufferSettings& framebufferSettings)
    {
        // add custom hash function faster than std::hash
        onyxU64 framebufferHash = Hash::FNV1aHash64(std::bit_cast<const onyxU8*>(&framebufferSettings), sizeof(FramebufferSettings));

        auto it = m_Cache.find(framebufferHash);
        if (it != m_Cache.end())
            return it->second;

        FramebufferHandle framebufferHandle = m_GraphicsApi.CreateFramebuffer(framebufferSettings);
        m_Cache[framebufferHash] = framebufferHandle;
        return framebufferHandle;
    }
}

