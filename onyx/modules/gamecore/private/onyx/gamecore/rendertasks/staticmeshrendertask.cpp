#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>

#include <onyx/gamecore/scene/sceneframedata.h>
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
            Graphics::ShaderEffectHandle shaderEffect = shaderGraph.GetShaderEffect();

            BindResources(shaderEffect, context.Graph.GetResourceCache(), context.FrameContext);
            hasBegun = true;
        }
    }

    void StaticMeshRenderGraphNode::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const Graphics::FrameContext& frameContext = context.FrameContext;
        //const Graphics::ViewConstants& viewConstants = frameContext.ViewConstants;

        if (frameContext.FrameData == nullptr)
            return;

        if (hasBegun == false)
            return;

        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);

        /*struct PushConstants
        {
            Vector4u32 m_TextureIndices;

            Vector4u32 ClusterSize;

            onyxF32 Scale;
            onyxF32 Bias;
            onyxU32 EnableDebug;
            onyxF32 Padding = 0.0f;
        };

        PushConstants constants;
        constants.ClusterSize =
        {
            Graphics::CLUSTER_X,
            Graphics::CLUSTER_Y,
            Graphics::CLUSTER_Z,
            static_cast<onyxU32>(std::ceil(viewConstants.Viewport[0] / Graphics::CLUSTER_X))
        };

        const onyxF32 nearFarLog = log2(viewConstants.Far / viewConstants.Near);
        constants.Scale = Graphics::CLUSTER_Z / nearFarLog;
        constants.Bias = -(Graphics::CLUSTER_Z * log2(viewConstants.Near) / nearFarLog);
        constants.EnableDebug = 0;*/

        onyxU32 instanceOffset = 0;
        
        for (const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls)
        {
            const Graphics::ShaderGraph& shaderGraph = *drawCall.Material;
            Graphics::ShaderEffectHandle shaderEffect = shaderGraph.GetShaderEffect();

            //shaderGraph.Render(context.FrameContext, commandBuffer);

            NodeGraph::GraphRunner runner(shaderGraph.GetNodeGraph());
            runner.Prepare();
            runner.Update(0);

            commandBuffer.BindShaderEffect(shaderGraph.GetShaderEffect());

            const Graphics::ShaderGraphTextures& shaderTextures = runner.GetContext().Get<Graphics::ShaderGraphTextures>();
            const DynamicArray<onyxU32>& textureIndices = shaderTextures.GetTextures();
            struct PushConstants
            {
                Vector3u32 LightClusterGridSize;
                onyxF32 LightClusterScale;

                Vector2u32 LightClusterSize;
                onyxF32 LightClusterBias;
                onyxU32 Debug;

                onyxU32 Textures[8] = {0};
            } constants;

            const Graphics::ViewConstants& viewConstants = frameContext.ViewConstants;
            constants.LightClusterGridSize =
            {
                Graphics::CLUSTER_X,
                Graphics::CLUSTER_Y,
                Graphics::CLUSTER_Z
            };

            constants.LightClusterSize = {
                static_cast<onyxU32>(std::ceil(viewConstants.Viewport[0] / Graphics::CLUSTER_X)),
                static_cast<onyxU32>(std::ceil(viewConstants.Viewport[1] / Graphics::CLUSTER_Y))
            };

            const onyxF32 nearFarLog = log2(viewConstants.Far / viewConstants.Near);
            constants.LightClusterScale = Graphics::CLUSTER_Z / nearFarLog;
            constants.LightClusterBias = -(Graphics::CLUSTER_Z * log2(viewConstants.Near) / nearFarLog);
            constants.Debug = drawCall.EnableClusterDebug ? 1 : 0;
            constants.Debug |= drawCall.EnableLightClusters ? 2 : 0;
            std::copy_n(textureIndices.data(), textureIndices.size(), constants.Textures);

            commandBuffer.BindPushConstants(Graphics::ShaderStage::Fragment, 0, sizeof(PushConstants), &constants);

            const onyxU32 instanceCount = 1;

            commandBuffer.BindVertexBuffer(drawCall.VertexData, 0, 0);
            commandBuffer.BindIndexBuffer(drawCall.Indices, 0, Graphics::IndexType::uint32);

            //for (Matrix4<onyxF32> transformMatrix : drawCall.m_Transforms)
            {
                //commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(Matrix4<onyxF32>), &transformMatrix);
                commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, drawCall.Indices->GetProperties().m_Size / 4, instanceCount, 0, 0, instanceOffset);
                instanceOffset += instanceCount;
            }
        }
    }

    bool StaticMeshRenderGraphNode::IsEnabled()
    {
        return true;
    }
}
