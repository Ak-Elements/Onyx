#include <onyx/colors/dark.h>
#include <onyx/editor/rendertasks/gridtask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/colors/base.h>
#include <onyx/profiler/profiler.h>

namespace onyx {

GridRenderGraphNode::GridRenderGraphNode() {
    m_PipelineProperties.Shader = "engine:/shaders/editor/grid.oshader";

    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
    gbufferInfo.Format = rhi::TextureFormat::RGBA_FLOAT32;
}

void GridRenderGraphNode::OnBeginFrame( graphics::RenderGraphContext& context ) {
    uint64_t outputGlobalId = GetOutputPin().GetGlobalId().get();

    const node_graph::PinBase& gbufferRenderTargetPin = GetInputPin0();
    if( gbufferRenderTargetPin.IsConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.GetResource(
            gbufferRenderTargetPin.GetLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.GetResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }
}
void GridRenderGraphNode::OnRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    struct Constants {
        Matrix3x3f32 Rotation;

        uint32_t CellCount;
        float32 LodLevel;
        float32 AxisLineWidth;
        float32 MajorLineWidth;
        float32 MinorLineWidth;

        uint32_t MajorLineColor;
        uint32_t MinorLineColor;

        uint32_t XAxisColor;
        uint32_t YAxisColor;
        uint32_t ZAxisColor;
        uint32_t Flags;
    };

    GridSettings& settings = context.Graph.GetInput< GridSettings >();
    auto rotation = Rotor3f32::fromEulerAngles( settings.Rotation ).toMatrix3();
    rotation.inverse();

    float32 gridLodLevel = numericCast< float32 >( settings.LodLevel );
    if( settings.UseAutoLod ) {
        const float32 log10 = std::log( 10.0f );
        Vector3f32 cameraPositionGridSpace = rotation * context.FrameContext.ViewConstants.CameraPosition;
        gridLodLevel = std::log( std::abs( cameraPositionGridSpace.Y ) ) / log10;
    }

    Constants constants{ .Rotation = rotation,
                         .CellCount = settings.Cells,
                         .LodLevel = gridLodLevel,
                         .AxisLineWidth = 0.2f,
                         .MajorLineWidth = 0.15f,
                         .MinorLineWidth = 0.02f,

                         .MajorLineColor = onyx::colors::White.toRGBA(),
                         .MinorLineColor = onyx::colors::White.toRGBA(),

                         .XAxisColor = colors::dark::Red9.toRGBA(),
                         .YAxisColor = colors::dark::Grass9.toRGBA(),
                         .ZAxisColor = colors::dark::Indigo10.toRGBA(),
                         .Flags = settings.UseAutoLod ? uint32_t( 0 ) : uint32_t( 1 ) };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}
} // namespace onyx
