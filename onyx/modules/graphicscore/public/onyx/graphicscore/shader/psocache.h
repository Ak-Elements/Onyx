#pragma once
#include <onyx/noncopyable.h>

#include <onyx/onyx_types.h>
#include <onyx/graphicscore/graphicstypes.h>


namespace Onyx::Graphics
{
    struct PipelineProperties;
    class ShaderInstance;

    class GraphicsSystem;

    class PsoCache : NonCopyable
    {
    public:
        void Clear();

        bool GetShaderEffect(ShaderEffectKey key, Reference<ShaderInstance>& outPso);

    private:
        HashMap<onyxU64, Reference<ShaderInstance>> m_ShaderEffects;
    };
}
