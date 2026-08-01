#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in3out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::graphics::render_graph_nodes {

class CreateLightClusters : public node_graph::FixedPinNode1Out< RenderGraphFixedShaderNode, rhi::BufferHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::CreateLightClusters";
    StringId32 getTypeId() const override { return TypeId; }

    CreateLightClusters();

  private:
    void onInit( rhi::GraphicsSystem& graphicsSystem, RenderGraphResourceCache& resourceCache ) override;

    void onBeginFrame( RenderGraphContext& context ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

  private:
    struct Constants {
        Matrix4x4f32 InverseProjection;

        Vector2f32 Viewport;
        float32 ZNear = 0.0f;
        float32 ZFar = 1.0f;

        uint64_t Clusters;
        Vector2u32 ClusterSize;
    };

    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_lightClustersStorageBuffers;
};

} // namespace onyx::graphics::render_graph_nodes
