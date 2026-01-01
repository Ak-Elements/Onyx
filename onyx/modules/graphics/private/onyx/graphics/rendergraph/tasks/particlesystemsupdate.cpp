#include <onyx/graphics/rendergraph/tasks/particlesystemsupdate.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    void ParticleSystemUpdateRenderGraphNode::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_UNUSED(context);
        ONYX_UNUSED(commandBuffer);
    }

#if ONYX_IS_EDITOR
    StringView ParticleSystemUpdateRenderGraphNode::GetPinName(StringId32 /*pinId*/) const
    {
        return "Execute";
    }
#endif
}
