#include <onyx/editor/rendertasks/gridtask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/colors/base.h>
#include <onyx/profiler/profiler.h>

namespace onyx {

namespace {
EngineVariable< uint32_t > g_gridScale{ "Editor/Grid/Scale", 1, 1 };
EngineVariable< bool > g_lockGrid{ "Editor/Grid/Lock", false };
} // namespace

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
void GridRenderGraphNode::OnRender( graphics::RenderGraphContext& /*context*/, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    struct Constants {
        uint32_t GridCellCount;
        uint GridScale;
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

    Constants constants{ .GridCellCount = 10,
                         .GridScale = g_gridScale.get(),
                         .AxisLineWidth = 0.15f,
                         .MajorLineWidth = 0.15f,
                         .MinorLineWidth = 0.02f,

                         .MajorLineColor = onyx::colors::White.toRGBA(),
                         .MinorLineColor = onyx::colors::White.toRGBA(),

                         .XAxisColor = onyx::colors::Red.toRGBA(),
                         .YAxisColor = onyx::colors::Green.toRGBA(),
                         .ZAxisColor = onyx::colors::Blue.toRGBA(),
                         .Flags = g_lockGrid.get() ? uint32_t( 1 ) : uint32_t( 0 ) };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}
} // namespace onyx
