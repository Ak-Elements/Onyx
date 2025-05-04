#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    void CreateTransmittanceRenderGraphNode::OnRender(RenderGraphContext& /*context*/, CommandBuffer& commandBuffer)
    {
        commandBuffer.Draw(PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }

#if ONYX_IS_EDITOR
    StringView CreateTransmittanceRenderGraphNode::GetPinName(StringId32 /*pinId*/) const
    {
        return "Transmittance";
    }
#endif
}
