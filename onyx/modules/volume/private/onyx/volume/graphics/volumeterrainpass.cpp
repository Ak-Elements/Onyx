#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
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

    uint64_t outputGlobalId = getOutputPin( 0 )->getGlobalId().get();

    const node_graph::PinBase* gbufferRenderTargetPin = getInputPinByLocalId( InPin0::LocalId );
    if( gbufferRenderTargetPin->isConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.getResource(
            gbufferRenderTargetPin->getLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.getResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }

    VolumeTerrainInstance& instance = context.Graph.getInput< VolumeTerrainInstance >();
    if( instance.Shader.isValid() == false )
        return;
    bindResources( instance.Shader, context.Graph.getResourceCache(), context.FrameContext );
}

void VolumeTerrainPass::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    return;
    ONYX_PROFILE_FUNCTION;

    VolumeTerrainInstance& instance = context.Graph.getInput< VolumeTerrainInstance >();
    if( instance.Shader.isValid() == false )
        return;
    commandBuffer.bindShaderEffect( instance.Shader );

    struct PushConstants {
        uint64_t ViewConstants;
        uint64_t VolumeSourcesList;
        uint64_t VolumeSourcesData;

        // float LightClusterGridSizeX;
        // float LightClusterGridSizeY;
        // float LightClusterGridSizeZ;
        // float LightClusterScale;

        // Vector2u32 LightClusterSize;
        // float LightClusterBias;
        uint TextureId0;
        uint TextureId1;
        uint TextureId2;
    };

    // const rhi::ViewConstants& viewConstants = context.FrameContext.ViewConstants;
    PushConstants constants{
        .ViewConstants = context.FrameContext.Api->getViewConstantsBuffer().getGpuAddress(),
        .VolumeSourcesList = instance.VolumeSources.getGpuAddress(),
        .VolumeSourcesData = instance.VolumeSourcesData.getGpuAddress(),
    };

    // constants.LightClusterGridSizeX = graphics::render_graph_nodes::ClusterX;
    // constants.LightClusterGridSizeY = graphics::render_graph_nodes::ClusterY;
    // constants.LightClusterGridSizeZ = graphics::render_graph_nodes::ClusterZ;

    // constants.LightClusterSize = {
    //     static_cast< uint32_t >( std::ceil( viewConstants.Viewport[ 0 ] / graphics::render_graph_nodes::ClusterX ) ),
    //     static_cast< uint32_t >( std::ceil( viewConstants.Viewport[ 1 ] / graphics::render_graph_nodes::ClusterY ) )
    //     };
    //
    // const float32 nearFarLog = std::log2( viewConstants.Far / viewConstants.Near );
    // constants.LightClusterScale = graphics::render_graph_nodes::ClusterZ / nearFarLog;
    // constants.LightClusterBias = -( graphics::render_graph_nodes::ClusterZ * std::log2( viewConstants.Near ) /
    //                                 nearFarLog );

    constants.TextureId0 = instance.TextureIndex0;
    constants.TextureId1 = instance.TextureIndex1;
    constants.TextureId2 = instance.TextureIndex2;
    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

} // namespace onyx::volume
