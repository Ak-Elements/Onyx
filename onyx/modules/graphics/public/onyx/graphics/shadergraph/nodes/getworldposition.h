#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>

namespace onyx::graphics::shader_graph_nodes {
class GetWorldPositionNode : public node_graph::FixedPinNode_1_Out< ShaderGraphNode, Vector4f32 > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::GetWorldPosition";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    void DoGenerateShader( const node_graph::ExecutionContext&, rhi::ShaderGenerator& ) const override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::graphics::shader_graph_nodes
