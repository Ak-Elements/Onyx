#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::graphics::render_graph_nodes {
class GetViewConstantsNode : public node_graph::FixedPinNode1Out< IRenderGraphNode, rhi::BufferHandle > {
    using Super = node_graph::FixedPinNode1Out< IRenderGraphNode, rhi::BufferHandle >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::GetViewConstants";
    StringId32 getTypeId() const override { return TypeId; }

    void init( assets::AssetSystem& assetSystem,
               rhi::GraphicsSystem& api,
               RenderGraphResourceCache& resourceCache ) override;
    void beginFrame( RenderGraphContext& context ) override;

    bool isEnabled() const override { return true; }

#if ONYX_IS_EDITOR
  private:
    StringView getPinName( StringId32 pinId ) const override {
        {
            switch( pinId )
            case Super::OutPin::LocalId:
                return "ViewConstants";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }

  private:
#endif
  private:
    InplaceArray< rhi::BufferHandle, rhi::MaxFramesInFlight > m_ViewConstantsUniformBuffers;
};
} // namespace onyx::graphics::render_graph_nodes
