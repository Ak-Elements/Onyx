#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in2out.h>

namespace onyx::graphics::shader_graph_nodes {
class SimplexNoise2DNode : public node_graph::FixedPinNode1In2Out< ShaderGraphNode, Vector2f32, float32, Vector2f32 > {
  private:
    using Super = node_graph::FixedPinNode1In2Out< ShaderGraphNode, Vector2f32, float32, Vector2f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::SimplexNoise2DShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    SimplexNoise2DNode() = default;

    void onUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif
};

class SimplexNoise3DNode : public node_graph::FixedPinNode1In2Out< ShaderGraphNode, Vector3f32, float32, Vector3f32 > {
  private:
    using Super = node_graph::FixedPinNode1In2Out< ShaderGraphNode, Vector3f32, float32, Vector3f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::SimplexNoise3DShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    SimplexNoise3DNode() = default;

    void onUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::graphics::shader_graph_nodes
