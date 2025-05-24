#pragma once
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    static constexpr onyxU8 CLUSTER_X = 16;
    static constexpr onyxU8 CLUSTER_Y = 9;
    static constexpr onyxU8 CLUSTER_Z = 24;
    static constexpr onyxU32 MAX_LIGHTS_PER_CLUSTER = 100;

    class CreateLightClusters : public NodeGraph::FixedPinNode_1_Out<RenderGraphFixedShaderNode, BufferHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::CreateLightClusters";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnInit(GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;

        void OnBeginFrame(const RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

    private:
        struct Constants
        {
            Matrix4<onyxF32> InverseProjection;

            Vector2f Viewport;
            onyxF32 zNear;
            onyxF32 zFar;

            Vector2u32 ClusterSize;
            Vector2u32 Padding;
        };

        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_LightClustersStorageBuffers;
    };

    class UpdateLightClustersRenderGraphNode : public NodeGraph::FixedPinNode_1_In_3_Out<RenderGraphFixedShaderNode, BufferHandle, BufferHandle, BufferHandle, BufferHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::UpdateLightClusters";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = NodeGraph::FixedPinNode_1_In_3_Out<RenderGraphFixedShaderNode, BufferHandle, BufferHandle, BufferHandle, BufferHandle>;

        void OnInit(GraphicsApi & api, RenderGraphResourceCache& resourceCache) override;

        void OnBeginFrame(const RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
                case InPin::LocalId: return "Light Clusters";
                case OutPin0::LocalId: return "Light Grid";
                case OutPin1::LocalId: return "Light Indices";
                case OutPin2::LocalId: return "Lights";
                default:
                    ONYX_ASSERT(false, "Invalid local pin id.");
                    return "Invalid";
            }
        }
#endif

    private:
        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_LightIndexListSSBO;
        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_LightGridSSBO;
        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_LightIndexGlobalCountSSBO;

        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_LightsStorageBuffers;
    };


}
