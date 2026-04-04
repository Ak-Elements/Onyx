#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in3out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::graphics::render_graph_nodes {
static constexpr uint8_t ClusterX = 16;
static constexpr uint8_t ClusterY = 9;
static constexpr uint8_t ClusterZ = 24;
static constexpr uint32_t MaxLightsPerCluster = 100;

class CreateLightClusters : public node_graph::FixedPinNode_1_Out< RenderGraphFixedShaderNode, rhi::BufferHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::CreateLightClusters";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    void OnInit( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) override;

    void OnBeginFrame( RenderGraphContext& context ) override;
    void OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

  private:
    struct Constants {
        Matrix4x4f32 InverseProjection;

        Vector2f32 Viewport;
        float32 ZNear = 0.0f;
        float32 ZFar = 1.0f;

        Vector2u32 ClusterSize;
        Vector2u32 Padding;
    };

    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_LightClustersStorageBuffers;
};

class UpdateLightClustersRenderGraphNode : public node_graph::FixedPinNode_1_In_3_Out< RenderGraphFixedShaderNode,
                                                                                       rhi::BufferHandle,
                                                                                       rhi::BufferHandle,
                                                                                       rhi::BufferHandle,
                                                                                       rhi::BufferHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::UpdateLightClusters";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = node_graph::FixedPinNode_1_In_3_Out< RenderGraphFixedShaderNode,
                                                       rhi::BufferHandle,
                                                       rhi::BufferHandle,
                                                       rhi::BufferHandle,
                                                       rhi::BufferHandle >;

    void OnInit( rhi::GraphicsSystem& graphicsSystem, RenderGraphResourceCache& resourceCache ) override;

    void OnBeginFrame( RenderGraphContext& context ) override;
    void OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
    StringView GetPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin::LocalId:
            return "Light Clusters";
        case OutPin0::LocalId:
            return "Light Grid";
        case OutPin1::LocalId:
            return "Light Indices";
        case OutPin2::LocalId:
            return "Lights";
        default:
            ONYX_ASSERT( false, "Invalid local pin id." );
            return "Invalid";
        }
    }
#endif

  private:
    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_LightIndexListSSBO;
    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_LightGridSSBO;
    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_LightIndexGlobalCountSSBO;

    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_LightsStorageBuffers;
};

} // namespace onyx::graphics::render_graph_nodes
