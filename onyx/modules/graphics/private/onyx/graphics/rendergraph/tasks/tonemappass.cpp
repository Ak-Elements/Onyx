#include <onyx/graphics/rendergraph/tasks/tonemappass.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>

namespace onyx::graphics::render_graph_nodes {
void ToneMapPass::onInit( assets::AssetSystem& assetSystem, rhi::GraphicsSystem&, RenderGraphResourceCache& ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId( "engine:/shaders/post/tonemap.slang" );
}

void ToneMapPass::onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t texturePinGlobalId = getInputPin().getLinkedPinGlobalId().get();

    const RenderGraphResource& inTextureResource = context.Graph.getResourceCache()[ texturePinGlobalId ];
    const rhi::TextureHandle& inTextureHandle = std::get< rhi::TextureHandle >( inTextureResource.Handle );

    struct PushConstants {
        uint32_t TonemapFunction;
    };

    PushConstants constants{ context.FrameContext.TonemapFunctor };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, inTextureHandle.Texture->GetIndex(), 1 );
}
} // namespace onyx::graphics::render_graph_nodes
