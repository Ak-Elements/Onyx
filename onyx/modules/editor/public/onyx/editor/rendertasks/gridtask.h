#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx {
struct GridSettings {
    EulerRadiansF32 Rotation;
    float32 Offset{ 0.0f };
    int32_t LodLevel{ 0 };
    uint32_t Cells{ 10 };
    bool UseAutoLod{ true };
};

class GridRenderGraphNode : public node_graph::FixedPinNode_2_In_1_Out< graphics::RenderGraphFixedShaderNode,
                                                                        rhi::TextureHandle,
                                                                        rhi::BufferHandle,
                                                                        rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::editor::render_graph_nodes::EditorSceneGridPass";
    StringId32 GetTypeId() const override { return TypeId; }

    GridRenderGraphNode();

  private:
    using Super = node_graph::FixedPinNode_2_In_1_Out< graphics::RenderGraphFixedShaderNode,
                                                       rhi::TextureHandle,
                                                       rhi::BufferHandle,
                                                       rhi::TextureHandle >;

    void OnBeginFrame( graphics::RenderGraphContext& context ) override;
    void OnRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case Super::InPin0::LocalId:
            return "Render Target";
        case Super::InPin1::LocalId:
            return "ViewConstants";
        case Super::OutPin::LocalId:
            return "Render Target";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};
} // namespace onyx
