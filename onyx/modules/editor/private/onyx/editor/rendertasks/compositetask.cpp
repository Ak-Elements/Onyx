#include <onyx/editor/rendertasks/compositetask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>

namespace onyx {
void CompositeRenderGraphNode::OnInit( rhi::GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/ ) {
    /*Graphics::RenderGraphResourceInfo& input = m_Inputs.emplace_back();
    input.Id = hash::FNV1aHash32("grid");
    input.Name = "grid";
    input.Type = Graphics::RenderGraphResourceType::Texture;*/

    // Graphics::RenderGraphResourceInfo& output = m_Outputs.emplace_back();
    // output.Id =  hash::FNV1aHash32("final");
    // output.Name = "final";
    // output.Type = Graphics::RenderGraphResourceType::Attachment;

    /*Graphics::RenderGraphResource& resource = resourceCache[output.Id];
    resource.Info = output;*/

    /*Graphics::RenderGraphTextureResourceInfo textureInfo;
    textureInfo.Format = Graphics::TextureFormat::BGRA_UNORM8;
    textureInfo.HasSize = false;
    resource.Properties = textureInfo;*/
}

#if ONYX_IS_EDITOR

#endif
void CompositeRenderGraphNode::OnRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    struct PushConstant {
        uint32_t TextureIndices[ 8 ];
        uint32_t Count = 0;
    } constants;

    const node_graph::PinBase* gridTextureInPin = GetInputPin( 0 );
    if ( gridTextureInPin->IsConnected() ) {
        const graphics::RenderGraphResource& resource = context.Graph.GetResource(
            gridTextureInPin->GetLinkedPinGlobalId().get() );
        const rhi::TextureHandle& gridTextureHandle = std::get< rhi::TextureHandle >( resource.Handle );
        constants.TextureIndices[ 1 ] = gridTextureHandle.Texture->GetIndex();
        ++constants.Count;
    }

    const node_graph::PinBase* gbufferTextureInPin = GetInputPin( 1 );
    if ( gbufferTextureInPin->IsConnected() ) {
        const graphics::RenderGraphResource& gbufferResource = context.Graph.GetResource(
            gbufferTextureInPin->GetLinkedPinGlobalId().get() );
        const rhi::TextureHandle& gbufferTextureHandle = std::get< rhi::TextureHandle >( gbufferResource.Handle );
        constants.TextureIndices[ 0 ] = gbufferTextureHandle.Texture->GetIndex();
        ++constants.Count;
    }

    const node_graph::PinBase* fontTextureInPin = GetInputPin( 2 );
    if ( fontTextureInPin->IsConnected() ) {
        const graphics::RenderGraphResource& font3dResource = context.Graph.GetResource(
            fontTextureInPin->GetLinkedPinGlobalId().get() );
        const rhi::TextureHandle& font3dTextureHandle = std::get< rhi::TextureHandle >( font3dResource.Handle );
        constants.TextureIndices[ 2 ] = font3dTextureHandle.Texture->GetIndex();
        ++constants.Count;
    }

    commandBuffer.BindPushConstants( rhi::ShaderStage::Fragment, 0, constants );
    commandBuffer.Draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}
} // namespace onyx
