#include <onyx/volume/graphics/previewterrainedit.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>

#include <onyx/profiler/profiler.h>

namespace onyx::volume {
namespace {
static constexpr StringId64 HIT_BUFFER_RESOURCE_ID = "hit buffer";
}

Vector3f32 PreviewTerrainEditPass::BrushSize{ 0.0f };
uint16_t PreviewTerrainEditPass::BrushType = 0;
uint16_t PreviewTerrainEditPass::BrushOperation = 0;

PreviewTerrainEditPass::PreviewTerrainEditPass() {
    m_PipelineProperties.Shader = "engine:/shaders/volume/render_terrain_brush.oshader";

    m_InputAttachmentInfos.emplace_back(); // buffer
    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
}

void PreviewTerrainEditPass::OnBeginFrame( graphics::RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t outputGlobalId = GetOutputPin( 0 )->GetGlobalId().get();

    const node_graph::PinBase* gbufferRenderTargetPin = GetInputPinByLocalId( InPin1::LocalId );
    if( gbufferRenderTargetPin->IsConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.GetResource(
            gbufferRenderTargetPin->GetLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.GetResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }
}

void PreviewTerrainEditPass::OnRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    if( context.Graph.HasResource( HIT_BUFFER_RESOURCE_ID ) == false ) {
        return;
    }

    struct PushConstants {
        uint64_t HitPositionBuffer;
        uint32_t DepthTextureIndex;
        uint32_t BrushType;

        Vector3f32 BrushSize;
        uint32_t BrushOperation;
    };

    PushConstants constants;
    const graphics::RenderGraphResource& hitBufferResource = context.Graph.GetResource( HIT_BUFFER_RESOURCE_ID );
    const rhi::BufferHandle& buffer = std::get< rhi::BufferHandle >( hitBufferResource.Handle );
    constants.HitPositionBuffer = buffer.Buffer->GetGpuAddress();

    const graphics::RenderGraphResource& depthTextureResource = context.Graph.GetResource(
        graphics::DEPTH_RESOURCE_ID );
    const rhi::TextureHandle& depthTexture = std::get< rhi::TextureHandle >( depthTextureResource.Handle );
    constants.DepthTextureIndex = depthTexture.Texture->GetIndex();

    constants.BrushSize = BrushSize;
    constants.BrushType = BrushType;
    constants.BrushOperation = BrushOperation;

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 6, 0, 1 );
}
} // namespace onyx::volume
