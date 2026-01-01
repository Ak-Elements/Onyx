#include <onyx/graphics/rendergraph/tasks/particlesystemsrender.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    ParticleSystemsRenderRenderGraphNode::ParticleSystemsRenderRenderGraphNode()
    {
        // setup render shader
    }

    void ParticleSystemsRenderRenderGraphNode::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_UNUSED(context);
        ONYX_UNUSED(commandBuffer);
    }

#if ONYX_IS_EDITOR
    StringView ParticleSystemsRenderRenderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin0::LocalId: return "Execute";
            case InPin1::LocalId: return "Render Target";
            case OutPin::LocalId: return "Out Render Target";
        }

        return "";
    }
#endif
}
