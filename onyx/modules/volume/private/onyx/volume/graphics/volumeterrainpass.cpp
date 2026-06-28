#include <onyx/volume/graphics/volumeterrainpass.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/profiler/profiler.h>

namespace onyx::volume {

VolumeTerrainPass::VolumeTerrainPass() {
    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_inputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
    graphics::RenderGraphTextureResourceInfo& depthTargetInfo = m_inputAttachmentInfos.emplace_back();
    depthTargetInfo.Type = graphics::RenderGraphResourceType::Attachment;
}

void VolumeTerrainPass::onBeginFrame( graphics::RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t outputGlobalId = GetOutputPin( 0 )->GetGlobalId().get();

    const node_graph::PinBase* gbufferRenderTargetPin = GetInputPinByLocalId( InPin0::LocalId );
    if( gbufferRenderTargetPin->IsConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.getResource(
            gbufferRenderTargetPin->GetLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.getResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }
}

void VolumeTerrainPass::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    VolumeTerrainInstance& instance = context.Graph.getInput< VolumeTerrainInstance >();
    if( instance.Shader.isValid() == false )
        return;
    commandBuffer.bindShaderEffect( instance.Shader );

    struct PushConstants {
        uint64_t ViewConstants;
        uint64_t VolumeSourcesList;
        uint64_t VolumeSourcesData;

        float LightClusterScale;

        Vector2u32 LightClusterSize;
        float LightClusterBias;
    };

    PushConstants constants{ .ViewConstants = context.FrameContext.Api->getViewConstantsBuffer().GetGpuAddress(),
                             .VolumeSourcesList = instance.VolumeSources.GetGpuAddress(),
                             .VolumeSourcesData = instance.VolumeSourcesData.GetGpuAddress() };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

} // namespace onyx::volume
