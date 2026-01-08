#include <onyx/rhi/framebuffercache.h>

#include <onyx/hash.h>
#include <onyx/rhi/framebuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/vulkan/graphicsapi.h>

namespace Onyx::Graphics
{
    FramebufferCache::FramebufferCache(GraphicsSystem& graphicsSystem)
        : m_GraphicsSystem(graphicsSystem)
    {
    }

    void FramebufferCache::Clear()
    {
        m_Cache.clear();
    }

    FramebufferHandle FramebufferCache::GetOrCreateFramebuffer(const FramebufferSettings& framebufferSettings)
    {
        // add custom hash function faster than std::hash
        onyxU64 framebufferHash = framebufferSettings.Hash();

        auto it = m_Cache.find(framebufferHash);
        if (it != m_Cache.end())
            return it->second;

        FramebufferHandle framebufferHandle = m_GraphicsSystem.CreateFramebuffer(framebufferSettings);
        m_Cache[framebufferHash] = framebufferHandle;
        return framebufferHandle;
    }
}

