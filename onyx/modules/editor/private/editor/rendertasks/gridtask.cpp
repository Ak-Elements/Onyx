#include <editor/rendertasks/gridtask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>

#include <onyx/profiler/profiler.h>

namespace Onyx
{
    void GridRenderGraphNode::OnRender(Graphics::RenderGraphContext& /*context*/, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        commandBuffer.Draw(Graphics::PrimitiveTopology::Triangle, 0, 6, 0, 1);
    }
}
 