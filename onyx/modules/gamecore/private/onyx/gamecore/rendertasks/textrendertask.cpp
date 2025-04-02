#include <onyx/gamecore/rendertasks/textrendertask.h>
#include <onyx/graphics/textureasset.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>

#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::GameCore
{
    void MSDFFontRenderPass::OnInit(Graphics::GraphicsApi& api, Graphics::RenderGraphResourceCache& /*resourceCache*/)
    {
        constexpr onyxU32 MAX_QUADS = 10000;
        constexpr onyxU32 MAX_VERTICES = MAX_QUADS * 4;
        constexpr onyxU32 MAX_INDICES = MAX_QUADS * 6;

        Graphics::BufferProperties vertexBufferProps;
        vertexBufferProps.m_Size = static_cast<onyxU32>(MAX_VERTICES * sizeof(Graphics::FontVertex));
        vertexBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Vertex);
        vertexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;

        api.CreateBuffer(m_VertexBuffer, vertexBufferProps);

        Graphics::BufferProperties indexBufferProps;
        indexBufferProps.m_Size = static_cast<onyxU32>(MAX_INDICES * sizeof(onyxU16));
        indexBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Index);
        indexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;

        onyxU16 indices[MAX_INDICES];
        onyxU16 offset = 0;
        for (onyxU32 i = 0; i < MAX_INDICES; i += 6)
        {
            indices[i + 0] = offset + static_cast<onyxU16>(0);
            indices[i + 1] = offset + static_cast<onyxU16>(1);
            indices[i + 2] = offset + static_cast<onyxU16>(2);

            indices[i + 3] = offset + static_cast<onyxU16>(2);
            indices[i + 4] = offset + static_cast<onyxU16>(3);
            indices[i + 5] = offset + static_cast<onyxU16>(0);

            offset += 4;
        }

        api.CreateBuffer(m_IndexBuffer, indexBufferProps);
        m_IndexBuffer->SetData(0, indices, static_cast<onyxU32>(MAX_INDICES * sizeof(onyxU16)));
    }

    void MSDFFontRenderPass::OnShutdown(Graphics::GraphicsApi& /*api*/)
    {
        m_VertexBuffer.Reset();
        m_IndexBuffer.Reset();
    }

    void MSDFFontRenderPass::OnPreRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& /*commandBuffer*/)
    {
        ONYX_PROFILE_FUNCTION;

        const Graphics::FrameContext& frameContext = context.FrameContext;

        if (frameContext.FrameData == nullptr)
            return;
            
        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);
        ONYX_UNUSED(sceneFrameData);
        //onyxU32 vertexOffset = 0;
        /*for (const TextDrawCall& drawCall : sceneFrameData.m_TextDrawCalls)
        {
            for (auto&& [_, vertices] : drawCall.VertexData)
            {
                m_VertexBuffer->SetData(vertexOffset, vertices->data(), static_cast<onyxS32>(sizeof(Graphics::FontVertex) * vertices->size()));
                vertexOffset += static_cast<onyxU32>(vertices->size());
            }
        }*/
    }

    void MSDFFontRenderPass::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const Graphics::FrameContext& frameContext = context.FrameContext;

        if (frameContext.FrameData == nullptr)
            return;

        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);

        commandBuffer.BindVertexBuffer(m_VertexBuffer, 0, 0);
        commandBuffer.BindIndexBuffer(m_IndexBuffer, 0, Graphics::IndexType::uint16);
        
        struct PushConstants
        {
            Matrix4<onyxF32> Transform;
        };

        PushConstants constants;
        ONYX_UNUSED(constants);
        ONYX_UNUSED(sceneFrameData);
        /*for (const TextDrawCall& drawCall : sceneFrameData.m_TextDrawCalls)
        {
            for (auto&& [textureIndex, vertices] : drawCall.VertexData)
            {
                constants.Transform = drawCall.Transforms[0];
                commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(PushConstants), &constants);
                commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, (static_cast<onyxU32>(vertices->size()) / 4u) * 6u, 1, 0, 0, textureIndex);
            }
        }*/
    }
}
