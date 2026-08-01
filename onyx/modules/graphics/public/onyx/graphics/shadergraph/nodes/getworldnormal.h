#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>

namespace onyx::graphics::shader_graph_nodes {
class GetWorldNormalNode : public node_graph::FixedPinNode1Out< ShaderGraphNode, Vector4f32 > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::GetWorldNormal";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    void doGenerateShader( const node_graph::ExecutionContext&, rhi::ShaderGenerator& ) const override;

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::graphics::shader_graph_nodes
