#include <onyx/volume/graphics/previewterrainedit.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/profiler/profiler.h>

namespace Onyx::Volume
{
    namespace
    {
        static constexpr StringId64 HIT_BUFFER_RESOURCE_ID = "hit buffer";
    }

    Vector3f32 PreviewTerrainEditPass::BrushSize { 0.0f };
    onyxU16 PreviewTerrainEditPass::BrushType = 0;
    onyxU16 PreviewTerrainEditPass::BrushOperation = 0;


    PreviewTerrainEditPass::PreviewTerrainEditPass()
    {
        m_PipelineProperties.Shader = "engine:/shaders/volume/render_terrain_brush.oshader";

        m_InputAttachmentInfos.emplace_back(); // buffer
        Graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
        gbufferInfo.Type = Graphics::RenderGraphResourceType::Attachment;

    }

    void PreviewTerrainEditPass::OnBeginFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 outputGlobalId = GetOutputPin(0)->GetGlobalId();

        const NodeGraph::PinBase* gbufferRenderTargetPin = GetInputPinByLocalId(InPin1::LocalId);
        if (gbufferRenderTargetPin->IsConnected())
        {
            const Graphics::RenderGraphResource& inputResource = context.Graph.GetResource(gbufferRenderTargetPin->GetLinkedPinGlobalId());
            Graphics::RenderGraphResource& outResource = context.Graph.GetResource(outputGlobalId);
            outResource.Handle = inputResource.Handle;
        }
    }

    void PreviewTerrainEditPass::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
        
        if (context.Graph.HasResource(HIT_BUFFER_RESOURCE_ID) == false)
        {
            return;
        }

        struct PushConstants
        {
            onyxU64 HitPositionBuffer;
            onyxU32 DepthTextureIndex;
            onyxU32 BrushType;

            Vector3f32 BrushSize;
            onyxU32 BrushOperation;
        };


        PushConstants constants;
        const Graphics::RenderGraphResource& hitBufferResource = context.Graph.GetResource(HIT_BUFFER_RESOURCE_ID);
        const Graphics::BufferHandle& buffer = std::get<Graphics::BufferHandle>(hitBufferResource.Handle);
        constants.HitPositionBuffer = buffer.Buffer->GetGpuAddress();

        const Graphics::RenderGraphResource& depthTextureResource = context.Graph.GetResource(Graphics::DEPTH_RESOURCE_ID);
        const Graphics::TextureHandle& depthTexture = std::get<Graphics::TextureHandle>(depthTextureResource.Handle);
        constants.DepthTextureIndex = depthTexture.Texture->GetIndex();

        constants.BrushSize = BrushSize;
        constants.BrushType = BrushType;
        constants.BrushOperation = BrushOperation;

        commandBuffer.BindPushConstants(Graphics::ShaderStage::Fragment, constants);
        commandBuffer.Draw(Graphics::PrimitiveTopology::Triangle, 0, 6, 0, 1);
    }
}
