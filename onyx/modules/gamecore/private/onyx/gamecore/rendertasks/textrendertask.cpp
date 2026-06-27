#include <onyx/gamecore/rendertasks/textrendertask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/vertex.h>

#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/profiler/profiler.h>

namespace onyx::game_core {
void MSDFFontRenderPass::onInit( rhi::GraphicsSystem& api, graphics::RenderGraphResourceCache& /*resourceCache*/ ) {
    constexpr uint32_t MAX_QUADS = 10000;
    constexpr uint32_t MAX_VERTICES = MAX_QUADS * 4;
    constexpr uint32_t MAX_INDICES = MAX_QUADS * 6;

    rhi::BufferProperties vertexBufferProps;
    vertexBufferProps.m_Size = static_cast< uint32_t >( MAX_VERTICES * sizeof( rhi::FontVertex ) );
    vertexBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Vertex );
    vertexBufferProps.m_CpuAccess = rhi::CPUAccess::Write;

    api.createBuffer( m_VertexBuffer, vertexBufferProps );

    rhi::BufferProperties indexBufferProps;
    indexBufferProps.m_Size = static_cast< uint32_t >( MAX_INDICES * sizeof( uint16_t ) );
    indexBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Index );
    indexBufferProps.m_CpuAccess = rhi::CPUAccess::Write;

    uint16_t indices[ MAX_INDICES ];
    uint16_t offset = 0;
    for( uint32_t i = 0; i < MAX_INDICES; i += 6 ) {
        indices[ i + 0 ] = offset + static_cast< uint16_t >( 0 );
        indices[ i + 1 ] = offset + static_cast< uint16_t >( 1 );
        indices[ i + 2 ] = offset + static_cast< uint16_t >( 2 );

        indices[ i + 3 ] = offset + static_cast< uint16_t >( 2 );
        indices[ i + 4 ] = offset + static_cast< uint16_t >( 3 );
        indices[ i + 5 ] = offset + static_cast< uint16_t >( 0 );

        offset += 4;
    }

    api.createBuffer( m_IndexBuffer, indexBufferProps );
    m_IndexBuffer.Buffer->SetData( 0, indices, static_cast< uint32_t >( MAX_INDICES * sizeof( uint16_t ) ) );
}

void MSDFFontRenderPass::onShutdown( rhi::GraphicsSystem& /*api*/ ) {
    m_VertexBuffer.Buffer.reset();
    m_IndexBuffer.Buffer.reset();
}

void MSDFFontRenderPass::onPreRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& /*commandBuffer*/ ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::FrameContext& frameContext = context.FrameContext;

    if( frameContext.FrameData == nullptr )
        return;

    std::ignore /*const SceneFrameData& sceneFrameData*/ = static_cast< const SceneFrameData& >(
        *frameContext.FrameData );
    // uint32_t vertexOffset = 0;
    /*for (const TextDrawCall& drawCall : sceneFrameData.m_TextDrawCalls)
    {
        for (auto&& [_, vertices] : drawCall.VertexData)
        {
            m_VertexBuffer->SetData(vertexOffset, vertices->data(), static_cast<int32_t>(sizeof(Graphics::FontVertex) *
    vertices->size())); vertexOffset += static_cast<uint32_t>(vertices->size());
        }
    }*/
}

void MSDFFontRenderPass::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::FrameContext& frameContext = context.FrameContext;

    if( frameContext.FrameData == nullptr )
        return;

    std::ignore /*const SceneFrameData& sceneFrameData*/ = static_cast< const SceneFrameData& >(
        *frameContext.FrameData );

    commandBuffer.bindVertexBuffer( m_VertexBuffer, 0, 0 );
    commandBuffer.bindIndexBuffer( m_IndexBuffer, 0, rhi::IndexType::uint16 );

    struct PushConstants {
        Matrix4< float32 > Transform;
    };

    // PushConstants constants;
    // ONYX_UNUSED( constants );
    // ONYX_UNUSED( sceneFrameData );
    /*for (const TextDrawCall& drawCall : sceneFrameData.m_TextDrawCalls)
    {
        for (auto&& [textureIndex, vertices] : drawCall.VertexData)
        {
            constants.Transform = drawCall.Transforms[0];
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(PushConstants), &constants);
            commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, (static_cast<uint32_t>(vertices->size()) /
    4u) * 6u, 1, 0, 0, textureIndex);
        }
    }*/
}
} // namespace onyx::game_core
