#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>

#include <onyx/rhi/commandbuffer.h>

#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/graphrunner.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::GameCore
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
        Graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
        gbufferInfo.Type = Graphics::RenderGraphResourceType::Attachment;
        gbufferInfo.Format = Graphics::TextureFormat::RGBA_FLOAT32;

        Graphics::RenderGraphTextureResourceInfo& depthtextureInfo = m_InputAttachmentInfos.emplace_back();
        depthtextureInfo.Type = Graphics::RenderGraphResourceType::Attachment;
    }

    void StaticMeshRenderGraphNode::OnBeginFrame(const Graphics::RenderGraphContext& context)
    {
        hasBegun = false;
        ONYX_PROFILE_FUNCTION;

        onyxU64 outputGlobalId = GetOutputPin(0)->GetGlobalId();

        const NodeGraph::PinBase* gbufferRenderTargetPin = GetInputPinByLocalId(GBufferTargetInPin::LocalId);
        if (gbufferRenderTargetPin->IsConnected())
        {
            const Graphics::RenderGraphResource& inputResource = context.Graph.GetResource(gbufferRenderTargetPin->GetLinkedPinGlobalId());
            Graphics::RenderGraphResource& outResource = context.Graph.GetResource(outputGlobalId);
            outResource.Handle = inputResource.Handle;
        }

        const Graphics::FrameContext& frameContext = context.FrameContext;
        if (frameContext.FrameData == nullptr)
            return;

        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);
        for (const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls)
        {
            const Graphics::MaterialShaderGraph& shaderGraph = *drawCall.Material;
            const Graphics::ShaderInstanceHandle& shaderInstance = shaderGraph.GetShader();

            BindResources(shaderInstance, context.Graph.GetResourceCache(), context.FrameContext);
            hasBegun = true;
        }

        for (const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls)
        {
            const Graphics::MaterialShaderGraph& shaderGraph = *indirectDrawCall.Material;
            const Graphics::ShaderInstanceHandle& shaderInstance = shaderGraph.GetShader();

            BindResources(shaderInstance, context.Graph.GetResourceCache(), context.FrameContext);
            hasBegun = true;
        }
    }

    void StaticMeshRenderGraphNode::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const Graphics::FrameContext& frameContext = context.FrameContext;
        
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
            commandBuffer.BindIndexBuffer(drawCall.Indices, 0, Graphics::IndexType::uint32);

            onyxU32 instanceOffset = 0;

            //for (Matrix4<onyxF32> transformMatrix : drawCall.m_Transforms)
            {
                commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, static_cast<onyxU32>(drawCall.Indices.Buffer->GetProperties().m_Size / 4), instanceCount, 0, 0, instanceOffset);
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
                commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, transformMatrix);
                commandBuffer.DrawIndirect(indirectDrawCall.DrawCommandBuffer, 1, 0, 0);
            }
        }
    }

    void StaticMeshRenderGraphNode::PrepareShaderGraph(Graphics::CommandBuffer& commandBuffer, const Graphics::FrameContext& frameContext, const Graphics::ShaderGraph& shaderGraph)
    {
        NodeGraph::GraphRunner runner(shaderGraph.GetNodeGraph());
        runner.Prepare();
        runner.Update(0);

        // TODO: Fix for other types
        const Graphics::MaterialShaderGraph& materialShader = static_cast<const Graphics::MaterialShaderGraph&>(shaderGraph);
        commandBuffer.BindShaderEffect(materialShader.GetShader());

        const Graphics::ShaderGraphTextures& shaderTextures = runner.GetContext().Get<Graphics::ShaderGraphTextures>();
        const DynamicArray<onyxU32>& textureIndices = shaderTextures.GetTextures();

        struct PushConstants
        {
            Vector3u32 LightClusterGridSize;
            onyxF32 LightClusterScale;

            Vector2u32 LightClusterSize;
            onyxF32 LightClusterBias;
            onyxU32 Debug;
        } generalConstants;

        const Graphics::ViewConstants& viewConstants = frameContext.ViewConstants;
        generalConstants.LightClusterGridSize =
        {
            Graphics::RenderGraphNodes::CLUSTER_X,
            Graphics::RenderGraphNodes::CLUSTER_Y,
            Graphics::RenderGraphNodes::CLUSTER_Z
        };

        generalConstants.LightClusterSize = {
            static_cast<onyxU32>(std::ceil(viewConstants.Viewport[0] / Graphics::RenderGraphNodes::CLUSTER_X)),
            static_cast<onyxU32>(std::ceil(viewConstants.Viewport[1] / Graphics::RenderGraphNodes::CLUSTER_Y))
        };

        const onyxF32 nearFarLog = log2(viewConstants.Far / viewConstants.Near);
        generalConstants.LightClusterScale = Graphics::RenderGraphNodes::CLUSTER_Z / nearFarLog;
        generalConstants.LightClusterBias = -(Graphics::RenderGraphNodes::CLUSTER_Z * log2(viewConstants.Near) / nearFarLog);
        generalConstants.Debug = 0;
        commandBuffer.BindPushConstants(Graphics::ShaderStage::Fragment, 64, generalConstants);
        if (textureIndices.empty() == false)
        {
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Fragment, 64 + sizeof(PushConstants), textureIndices);
        }
    }

    bool StaticMeshRenderGraphNode::IsEnabled()
    {
        return true;
    }
}
