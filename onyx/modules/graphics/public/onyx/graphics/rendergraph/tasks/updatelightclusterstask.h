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

class UpdateLightClustersRenderGraphNode : public node_graph::FixedPinNode1In3Out< RenderGraphFixedShaderNode,
                                                                                   rhi::BufferHandle,
                                                                                   rhi::BufferHandle,
                                                                                   rhi::BufferHandle,
                                                                                   rhi::BufferHandle > {
  public:
    static constexpr RenderGraphResourceId LightEnvironmentResourceId = hash::fnV1aHash< uint32_t >(
        "lightenvironment" );
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::UpdateLightClusters";
    StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = node_graph::FixedPinNode1In3Out< RenderGraphFixedShaderNode,
                                                   rhi::BufferHandle,
                                                   rhi::BufferHandle,
                                                   rhi::BufferHandle,
                                                   rhi::BufferHandle >;

    void onInit( assets::AssetSystem& assetSystem,
                 rhi::GraphicsSystem& graphicsSystem,
                 RenderGraphResourceCache& resourceCache ) override;

    void onBeginFrame( RenderGraphContext& context ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
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
    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_lightEnvironments;

    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_lightIndexListSsbo;
    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_lightGridSsbo;
    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_lightIndexGlobalCountSsbo;

    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_directionalLightsSsbo;
    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_pointLightsSsbo;
    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_spotLightsSsbo;
};

} // namespace onyx::graphics::render_graph_nodes
