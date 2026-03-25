#include <onyx/rhi/shader/psocache.h>

#include <onyx/rhi/graphicshandles.h>

namespace onyx::rhi {
// PipelineHandle PsoCache::GetOrCreatePipeline(GraphicsSystem& api, const PsoKey& key, ShaderEffectHandle& outPso)
//{
//     ONYX_UNUSED(api);
//     ONYX_UNUSED(outPso);
//     uint64_t psoKey = hash::FNV1aHash64(key.Rasterization);

//    psoKey = hash::FNV1aHash64(key.DepthStencil, psoKey);
//    psoKey = hash::FNV1aHash64(key.BlendState, psoKey);
//    psoKey = hash::FNV1aHash64(key.ColorWriteMask, psoKey);
//    psoKey = hash::FNV1aHash64(key.IsBlendEnabled, psoKey);
//    psoKey = hash::FNV1aHash64(key.IsSeperateBlend, psoKey);

//    auto it = m_Pipelines.find(psoKey);
//    if (it != m_Pipelines.end())
//        return it->second;

void PsoCache::Clear() {
    m_ShaderEffects.clear();
}

//    // TODO: maybe friend class and not expose CreateFramebuffer in the public interface?
//    //FramebufferHandle framebufferHandle = m_GraphicsSystem.CreateFramebuffer(framebufferSettings);
//    //m_Cache[framebufferHash] = framebufferHandle;
//    return PipelineHandle::Invalid();
//}
bool PsoCache::GetShaderEffect( ShaderEffectKey key, ShaderInstanceHandle& outPso ) {
    auto it = m_ShaderEffects.find( key );

    if ( it != m_ShaderEffects.end() ) {
        outPso = it->second;
        return true;
    }

    return false;
}
} // namespace onyx::rhi
