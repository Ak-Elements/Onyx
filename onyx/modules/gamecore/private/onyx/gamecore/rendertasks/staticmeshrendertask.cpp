#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>

#include <onyx/rhi/commandbuffer.h>

#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/graphrunner.h>
#include <onyx/profiler/profiler.h>

namespace onyx::game_core
{
    bool hasBegun = false;

    StaticMeshRenderGraphNode::StaticMeshRenderGraphNode()
    {
        AddInPin<ViewConstantsInPin>();
        AddInPin<GBufferTargetInPin>();
        AddInPin<DepthTextureInPin>();
        AddInPin<LightGridInPin>();
        AddInPin<LightIndicesInPin>();
        AddInPin<LightsInPin>();
        
        AddOutPin<OutPin>();

        m_InputAttachmentInfos.emplace_back();
        graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
        gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
        gbufferInfo.Format = rhi::TextureFormat::RGBA_FLOAT32;

        graphics::RenderGraphTextureResourceInfo& depthtextureInfo = m_InputAttachmentInfos.emplace_back();
        depthtextureInfo.Type = graphics::RenderGraphResourceType::Attachment;
    }

    void StaticMeshRenderGraphNode::OnBeginFrame(graphics::RenderGraphContext& context)
    {
        hasBegun = false;
        ONYX_PROFILE_FUNCTION;

        onyxU64 outputGlobalId = GetOutputPin(0)->GetGlobalId();

        const node_graph::PinBase* gbufferRenderTargetPin = GetInputPinByLocalId(GBufferTargetInPin::LocalId);
        if (gbufferRenderTargetPin->IsConnected())
        {
            const graphics::RenderGraphResource& inputResource = context.Graph.GetResource(gbufferRenderTargetPin->GetLinkedPinGlobalId());
            graphics::RenderGraphResource& outResource = context.Graph.GetResource(outputGlobalId);
            outResource.Handle = inputResource.Handle;
        }

        const rhi::FrameContext& frameContext = context.FrameContext;
        if (frameContext.FrameData == nullptr)
            return;

        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);
        for (const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls)
        {
            const graphics::MaterialShaderGraph& shaderGraph = *drawCall.Material;
            const rhi::ShaderInstanceHandle& shaderInstance = shaderGraph.GetShader();

            BindResources(shaderInstance, context.Graph.GetResourceCache(), context.FrameContext);
            hasBegun = true;
        }

        for (const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls)
        {
            const graphics::MaterialShaderGraph& shaderGraph = *indirectDrawCall.Material;
            const rhi::ShaderInstanceHandle& shaderInstance = shaderGraph.GetShader();

            BindResources(shaderInstance, context.Graph.GetResourceCache(), context.FrameContext);
            hasBegun = true;
        }
    }

    void StaticMeshRenderGraphNode::OnRender(graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const rhi::FrameContext& frameContext = context.FrameContext;
        
        if (frameContext.FrameData == nullptr)
            return;

        if (hasBegun == false)
            return;

        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);

        for (const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls)
        {
            PrepareShaderGraph(commandBuffer, context.FrameContext, *drawCall.Material);

            const onyxU32 instanceCount = 1;

            commandBuffer.BindVertexBuffer(drawCall.VertexData, 0, 0);
            commandBuffer.BindIndexBuffer(drawCall.Indices, 0, rhi::IndexType::uint32);

            onyxU32 instanceOffset = 0;

            for (Matrix4<onyxF32> transformMatrix : drawCall.Transforms)
            {
                commandBuffer.BindPushConstants(rhi::ShaderStage::Vertex, 0, transformMatrix);
                commandBuffer.DrawIndexed(rhi::PrimitiveTopology::Triangle, static_cast<onyxU32>(drawCall.Indices.Buffer->GetProperties().m_Size / 4), instanceCount, 0, 0, instanceOffset);
                //instanceOffset += instanceCount;
            }
        }

        if (sceneFrameData.m_StaticMeshIndirectDrawCalls.empty())
            return;

       
        const StaticMeshIndirectDrawCall& first = sceneFrameData.m_StaticMeshIndirectDrawCalls.front();
        PrepareShaderGraph(commandBuffer, context.FrameContext, *first.Material);

        for (const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls)
        {
            commandBuffer.BindVertexBuffer(indirectDrawCall.VertexData, 0, 0);

            for (Matrix4<onyxF32> transformMatrix : indirectDrawCall.Transforms)
            {
                commandBuffer.BindPushConstants(rhi::ShaderStage::Vertex, 0, transformMatrix);
                commandBuffer.DrawIndirect(indirectDrawCall.DrawCommandBuffer, 1, 0, 0);
            }
        }
    }

    void StaticMeshRenderGraphNode::PrepareShaderGraph(rhi::CommandBuffer& commandBuffer, const rhi::FrameContext& frameContext, const graphics::ShaderGraph& shaderGraph)
    {
        node_graph::GraphRunner runner(shaderGraph.GetNodeGraph());
        runner.Prepare();
        runner.Update(0);

        // TODO: Fix for other types
        const graphics::MaterialShaderGraph& materialShader = static_cast<const graphics::MaterialShaderGraph&>(shaderGraph);
        commandBuffer.BindShaderEffect(materialShader.GetShader());

        const graphics::ShaderGraphTextures& shaderTextures = runner.GetContext().Get<graphics::ShaderGraphTextures>();
        const DynamicArray<onyxU32>& textureIndices = shaderTextures.GetTextures();

        struct PushConstants
        {
            Vector3u32 LightClusterGridSize;
            onyxF32 LightClusterScale;

            Vector2u32 LightClusterSize;
            onyxF32 LightClusterBias;
            onyxU32 Debug;
        } generalConstants;

        const rhi::ViewConstants& viewConstants = frameContext.ViewConstants;
        generalConstants.LightClusterGridSize =
        {
            graphics::render_graph_nodes::CLUSTER_X,
            graphics::render_graph_nodes::CLUSTER_Y,
            graphics::render_graph_nodes::CLUSTER_Z
        };

        generalConstants.LightClusterSize = {
            static_cast<onyxU32>(std::ceil(viewConstants.Viewport[0] / graphics::render_graph_nodes::CLUSTER_X)),
            static_cast<onyxU32>(std::ceil(viewConstants.Viewport[1] / graphics::render_graph_nodes::CLUSTER_Y))
        };

        const onyxF32 nearFarLog = log2(viewConstants.Far / viewConstants.Near);
        generalConstants.LightClusterScale = graphics::render_graph_nodes::CLUSTER_Z / nearFarLog;
        generalConstants.LightClusterBias = -(graphics::render_graph_nodes::CLUSTER_Z * log2(viewConstants.Near) / nearFarLog);
        generalConstants.Debug = 0;
        commandBuffer.BindPushConstants(rhi::ShaderStage::Fragment, 64, generalConstants);
        if (textureIndices.empty() == false)
        {
            commandBuffer.BindPushConstants(rhi::ShaderStage::Fragment, 64 + sizeof(PushConstants), textureIndices);
        }
    }

    bool StaticMeshRenderGraphNode::IsEnabled()
    {
        return true;
    }
}
