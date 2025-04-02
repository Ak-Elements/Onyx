#include <onyx/graphics/shader/psocache.h>

#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
{
    //PipelineHandle PsoCache::GetOrCreatePipeline(GraphicsApi& api, const PsoKey& key, ShaderEffectHandle& outPso)
    //{
    //    ONYX_UNUSED(api);
    //    ONYX_UNUSED(outPso);
    //    onyxU64 psoKey = Hash::FNV1aHash64(key.Rasterization);

    //    psoKey = Hash::FNV1aHash64(key.DepthStencil, psoKey);
    //    psoKey = Hash::FNV1aHash64(key.BlendState, psoKey);
    //    psoKey = Hash::FNV1aHash64(key.ColorWriteMask, psoKey);
    //    psoKey = Hash::FNV1aHash64(key.IsBlendEnabled, psoKey);
    //    psoKey = Hash::FNV1aHash64(key.IsSeperateBlend, psoKey);

    //    auto it = m_Pipelines.find(psoKey);
    //    if (it != m_Pipelines.end())
    //        return it->second;

    void PsoCache::Clear()
    {
        m_ShaderEffects.clear();
    }

    //    // TODO: maybe friend class and not expose CreateFramebuffer in the public interface?
    //    //FramebufferHandle framebufferHandle = m_GraphicsApi.CreateFramebuffer(framebufferSettings);
    //    //m_Cache[framebufferHash] = framebufferHandle;
    //    return PipelineHandle::Invalid();
    //}
    bool PsoCache::GetShaderEffect(ShaderEffectKey key, ShaderEffectHandle& outPso)
    {
        auto it = m_ShaderEffects.find(key);

        if (it != m_ShaderEffects.end())
        {
            outPso = it->second;
            return true;
        }

        return false;
    }
}
