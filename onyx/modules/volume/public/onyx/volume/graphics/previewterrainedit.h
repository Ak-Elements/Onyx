#pragma once

#include <onyx/graphics/rendergraph/rendergraph.h> // TODO: REMOVE
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>

namespace onyx::volume {
class PreviewTerrainEditPass : public node_graph::FixedPinNode_2_In_1_Out< graphics::RenderGraphFixedShaderNode,
                                                                           rhi::BufferHandle,
                                                                           rhi::TextureHandle,
                                                                           rhi::TextureHandle > {
  public:
    // TODO: Move this to a blackboard / variable storage on the RenderGraph?
    static Vector3f32 BrushSize;
    static uint16_t BrushType;
    static uint16_t BrushOperation;

    static constexpr StringId32 TypeId = "onyx::volume::RenderGraph::PreviewTerrainEdit";
    StringId32 GetTypeId() const override { return TypeId; }

    PreviewTerrainEditPass();

    PreviewTerrainEditPass( const PreviewTerrainEditPass& ) = default;
    PreviewTerrainEditPass( PreviewTerrainEditPass&& ) = default;
    PreviewTerrainEditPass& operator=( const PreviewTerrainEditPass& ) = default;
    PreviewTerrainEditPass& operator=( PreviewTerrainEditPass&& ) = default;

  private:
    using Super = node_graph::FixedPinNode_2_In_1_Out< graphics::RenderGraphFixedShaderNode,
                                                       rhi::BufferHandle,
                                                       rhi::TextureHandle,
                                                       rhi::TextureHandle >;

    void OnBeginFrame( graphics::RenderGraphContext& ) override;
    void OnRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin0::LocalId:
            return "View Constants";
        case Super::InPin1::LocalId:
            return "Render Target";
        case Super::OutPin::LocalId:
            return "Out";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};
} // namespace onyx::volume
