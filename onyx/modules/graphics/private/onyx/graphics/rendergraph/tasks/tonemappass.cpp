#include <onyx/graphics/rendergraph/tasks/tonemappass.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>

namespace onyx::graphics::render_graph_nodes
{
    ToneMapPass::ToneMapPass()
    {
        m_PipelineProperties.Shader = "engine:/shaders/post/tonemap.oshader";
    }

    void ToneMapPass::OnRender(RenderGraphContext& context, rhi::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 texturePinGlobalId = GetInputPin().GetLinkedPinGlobalId();

        const RenderGraphResource& inTextureResource = context.Graph.GetResourceCache()[texturePinGlobalId];
        const rhi::TextureHandle& inTextureHandle = std::get<rhi::TextureHandle>(inTextureResource.Handle);

        struct PushConstants
        {
            onyxU32 TonemapFunction;
        };

        PushConstants constants;
        constants.TonemapFunction = context.FrameContext.TonemapFunctor;

        commandBuffer.BindPushConstants(rhi::ShaderStage::Fragment, 0, constants);
        commandBuffer.Draw(rhi::PrimitiveTopology::Triangle, 0, 3, inTextureHandle.Texture->GetIndex(), 1);
    }
}
