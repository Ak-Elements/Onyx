#include <onyx/editor/rendertasks/gridtask.h>

#include <onyx/rhi/commandbuffer.h>

#include <onyx/profiler/profiler.h>

namespace onyx
{
    void GridRenderGraphNode::OnRender(graphics::RenderGraphContext& /*context*/, rhi::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
        commandBuffer.Draw(rhi::PrimitiveTopology::Triangle, 0, 6, 0, 1);
    }
}
 