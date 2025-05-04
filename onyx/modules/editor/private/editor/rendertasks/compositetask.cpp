#include <editor/rendertasks/compositetask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>

namespace Onyx
{
    void CompositeRenderGraphNode::OnInit(Graphics::GraphicsApi& /*api*/ , RenderGraphResourceCache& /*resourceCache*/)
    {
        /*Graphics::RenderGraphResourceInfo& input = m_Inputs.emplace_back();
        input.Id = Hash::FNV1aHash32("grid");
        input.Name = "grid";
        input.Type = Graphics::RenderGraphResourceType::Texture;*/

        //Graphics::RenderGraphResourceInfo& output = m_Outputs.emplace_back();
        //output.Id =  Hash::FNV1aHash32("final");
        //output.Name = "final";
        //output.Type = Graphics::RenderGraphResourceType::Attachment;

        /*Graphics::RenderGraphResource& resource = resourceCache[output.Id];
        resource.Info = output;*/

        /*Graphics::RenderGraphTextureResourceInfo textureInfo;
        textureInfo.Format = Graphics::TextureFormat::BGRA_UNORM8;
        textureInfo.HasSize = false;
        resource.Properties = textureInfo;*/
    }

#if ONYX_IS_EDITOR
    
#endif
    void CompositeRenderGraphNode::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        struct PushConstant
        {
            onyxU32 TextureIndices[8];
            onyxU32 Count = 0;
        } constants;

        const NodeGraph::PinBase* gridTextureInPin = GetInputPin(0);
        if (gridTextureInPin->IsConnected())
        {
            const Graphics::RenderGraphResource& resource = context.Graph.GetResource(gridTextureInPin->GetLinkedPinGlobalId());
            const Graphics::TextureHandle& gridTextureHandle = std::get<Graphics::TextureHandle>(resource.Handle);
            constants.TextureIndices[1] = gridTextureHandle.Texture->GetIndex();
            ++constants.Count;
        }

        const NodeGraph::PinBase* gbufferTextureInPin = GetInputPin(1);
        if (gbufferTextureInPin->IsConnected())
        {
            const Graphics::RenderGraphResource& gbufferResource = context.Graph.GetResource(gbufferTextureInPin->GetLinkedPinGlobalId());
            const Graphics::TextureHandle& gbufferTextureHandle = std::get<Graphics::TextureHandle>(gbufferResource.Handle);
            constants.TextureIndices[0] = gbufferTextureHandle.Texture->GetIndex();
            ++constants.Count;
        }

        const NodeGraph::PinBase* fontTextureInPin = GetInputPin(2);
        if (fontTextureInPin->IsConnected())
        {
            const Graphics::RenderGraphResource& font3dResource = context.Graph.GetResource(fontTextureInPin->GetLinkedPinGlobalId());
            const Graphics::TextureHandle& font3dTextureHandle = std::get<Graphics::TextureHandle>(font3dResource.Handle);
            constants.TextureIndices[2] = font3dTextureHandle.Texture->GetIndex();
            ++constants.Count;
        }

        commandBuffer.BindPushConstants(Graphics::ShaderStage::Fragment, 0, sizeof(PushConstant), &constants);
        commandBuffer.Draw(Graphics::PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }
}
