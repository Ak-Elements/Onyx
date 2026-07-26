#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>

#include <onyx/rhi/commandbuffer.h>

#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/graphrunner.h>
#include <onyx/profiler/profiler.h>

namespace onyx::game_core {
bool g_hasBegun = false;

StaticMeshRenderGraphNode::StaticMeshRenderGraphNode() {
    AddInPin< ViewConstantsInPin >();
    AddInPin< GBufferTargetInPin >();
    AddInPin< DepthTextureInPin >();
    AddInPin< LightGridInPin >();
    AddInPin< LightIndicesInPin >();
    AddInPin< LightsInPin >();

    AddOutPin< OutPin >();

    m_inputAttachmentInfos.emplace_back();
    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_inputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
    gbufferInfo.Format = rhi::TextureFormat::RGBA_FLOAT32;

    graphics::RenderGraphTextureResourceInfo& depthtextureInfo = m_inputAttachmentInfos.emplace_back();
    depthtextureInfo.Type = graphics::RenderGraphResourceType::Attachment;
}

void StaticMeshRenderGraphNode::onBeginFrame( graphics::RenderGraphContext& context ) {
    g_hasBegun = false;
    ONYX_PROFILE_FUNCTION;

    uint64_t outputGlobalId = GetOutputPin( 0 )->GetGlobalId().get();

    const node_graph::PinBase* gbufferRenderTargetPin = GetInputPinByLocalId( GBufferTargetInPin::LocalId );
    if( gbufferRenderTargetPin->IsConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.getResource(
            gbufferRenderTargetPin->GetLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.getResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }

    const rhi::FrameContext& frameContext = context.FrameContext;
    if( frameContext.FrameData == nullptr )
        return;

    const SceneFrameData& sceneFrameData = static_cast< const SceneFrameData& >( *frameContext.FrameData );
    for( const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls ) {
        const graphics::MaterialShaderGraph& shaderGraph = *drawCall.Material;
        const rhi::ShaderInstanceHandle& shaderInstance = shaderGraph.getShader();

        bindResources( shaderInstance, context.Graph.getResourceCache(), context.FrameContext );
        g_hasBegun = true;
    }

    for( const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls ) {
        const graphics::MaterialShaderGraph& shaderGraph = *indirectDrawCall.Material;
        const rhi::ShaderInstanceHandle& shaderInstance = shaderGraph.getShader();

        bindResources( shaderInstance, context.Graph.getResourceCache(), context.FrameContext );
        g_hasBegun = true;
    }
}

void StaticMeshRenderGraphNode::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::FrameContext& frameContext = context.FrameContext;

    if( frameContext.FrameData == nullptr )
        return;

    if( g_hasBegun == false )
        return;

    const SceneFrameData& sceneFrameData = static_cast< const SceneFrameData& >( *frameContext.FrameData );

    for( const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls ) {
        prepareShaderGraph( commandBuffer, context.FrameContext, *drawCall.Material );

        const uint32_t instanceCount = 1;

        commandBuffer.bindVertexBuffer( drawCall.VertexData, 0, 0 );
        commandBuffer.bindIndexBuffer( drawCall.Indices, 0, rhi::IndexType::uint32 );

        uint32_t instanceOffset = 0;

        for( Matrix4< float32 > transformMatrix : drawCall.Transforms ) {
            commandBuffer.bindPushConstants( rhi::ShaderStage::Vertex, 0, transformMatrix );
            commandBuffer.drawIndexed( rhi::PrimitiveTopology::Triangle,
                                       static_cast< uint32_t >( drawCall.Indices.Buffer->GetProperties().m_Size / 4 ),
                                       instanceCount,
                                       0,
                                       0,
                                       instanceOffset );
            // instanceOffset += instanceCount;
        }
    }

    if( sceneFrameData.m_StaticMeshIndirectDrawCalls.empty() )
        return;

    const StaticMeshIndirectDrawCall& first = sceneFrameData.m_StaticMeshIndirectDrawCalls.front();
    prepareShaderGraph( commandBuffer, context.FrameContext, *first.Material );

    for( const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls ) {
        commandBuffer.bindVertexBuffer( indirectDrawCall.VertexData, 0, 0 );

        for( Matrix4< float32 > transformMatrix : indirectDrawCall.Transforms ) {
            commandBuffer.bindPushConstants( rhi::ShaderStage::Vertex, 0, transformMatrix );
            commandBuffer.drawIndirect( indirectDrawCall.DrawCommandBuffer, 1, 0, 0 );
        }
    }
}

void StaticMeshRenderGraphNode::prepareShaderGraph( rhi::CommandBuffer& commandBuffer,
                                                    const rhi::FrameContext& frameContext,
                                                    const graphics::ShaderGraph& shaderGraph ) {
    node_graph::GraphRunner runner( shaderGraph.getNodeGraph() );
    runner.Prepare();
    runner.Update( 0 );

    // TODO: Fix for other types
    const graphics::MaterialShaderGraph& materialShader = static_cast< const graphics::MaterialShaderGraph& >(
        shaderGraph );
    commandBuffer.bindShaderEffect( materialShader.getShader() );

    const graphics::ShaderGraphTextures& shaderTextures = runner.GetContext().Get< graphics::ShaderGraphTextures >();
    const DynamicArray< uint32_t >& textureIndices = shaderTextures.getTextures();

    struct PushConstants {
        Vector3u32 LightClusterGridSize;
        float32 LightClusterScale;

        Vector2u32 LightClusterSize;
        float32 LightClusterBias;
        uint32_t Debug;
    } generalConstants;

    const rhi::ViewConstants& viewConstants = frameContext.ViewConstants;
    generalConstants.LightClusterGridSize = { graphics::render_graph_nodes::ClusterX,
                                              graphics::render_graph_nodes::ClusterY,
                                              graphics::render_graph_nodes::ClusterZ };

    generalConstants.LightClusterSize = {
        static_cast< uint32_t >( std::ceil( viewConstants.Viewport[ 0 ] / graphics::render_graph_nodes::ClusterX ) ),
        static_cast< uint32_t >( std::ceil( viewConstants.Viewport[ 1 ] / graphics::render_graph_nodes::ClusterY ) ) };

    const float32 nearFarLog = log2( viewConstants.Far / viewConstants.Near );
    generalConstants.LightClusterScale = graphics::render_graph_nodes::ClusterZ / nearFarLog;
    generalConstants.LightClusterBias = -( graphics::render_graph_nodes::ClusterZ * log2( viewConstants.Near ) /
                                           nearFarLog );
    generalConstants.Debug = 0;
    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 64, generalConstants );
    if( textureIndices.empty() == false ) {
        commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 64 + sizeof( PushConstants ), textureIndices );
    }
}

bool StaticMeshRenderGraphNode::isEnabled() const {
    return true;
}
} // namespace onyx::game_core
