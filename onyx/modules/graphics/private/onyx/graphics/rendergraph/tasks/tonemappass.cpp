#include <onyx/graphics/rendergraph/tasks/tonemappass.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>

namespace onyx::graphics::render_graph_nodes {
ToneMapPass::ToneMapPass() {
    m_PipelineProperties.Shader = "engine:/shaders/post/tonemap.oshader";
}

void ToneMapPass::OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t texturePinGlobalId = GetInputPin().GetLinkedPinGlobalId().get();

    const RenderGraphResource& inTextureResource = context.Graph.GetResourceCache()[ texturePinGlobalId ];
    const rhi::TextureHandle& inTextureHandle = std::get< rhi::TextureHandle >( inTextureResource.Handle );

    struct PushConstants {
        uint32_t TonemapFunction;
    };

    PushConstants constants;
    constants.TonemapFunction = context.FrameContext.TonemapFunctor;

    commandBuffer.BindPushConstants( rhi::ShaderStage::Fragment, 0, constants );
    commandBuffer.Draw( rhi::PrimitiveTopology::Triangle, 0, 3, inTextureHandle.Texture->GetIndex(), 1 );
}
} // namespace onyx::graphics::render_graph_nodes
