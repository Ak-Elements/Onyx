#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in2out.h>

namespace onyx::graphics::shader_graph_nodes {
class SimplexNoise2DNode
    : public node_graph::FixedPinNode_1_In_2_Out< ShaderGraphNode, Vector2f32, float32, Vector2f32 > {
  private:
    using Super = node_graph::FixedPinNode_1_In_2_Out< ShaderGraphNode, Vector2f32, float32, Vector2f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::ShaderGraph::SimplexNoise2DShaderGraphNode";
    StringId32 GetTypeId() const override { return TypeId; }

    SimplexNoise2DNode() = default;

    void OnUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void DoGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    StringView GetPinName( StringId32 pinId ) const override;
#endif
};

class SimplexNoise3DNode
    : public node_graph::FixedPinNode_1_In_2_Out< ShaderGraphNode, Vector3f32, float32, Vector3f32 > {
  private:
    using Super = node_graph::FixedPinNode_1_In_2_Out< ShaderGraphNode, Vector3f32, float32, Vector3f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::ShaderGraph::SimplexNoise3DShaderGraphNode";
    StringId32 GetTypeId() const override { return TypeId; }

    SimplexNoise3DNode() = default;

    void OnUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void DoGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    StringView GetPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::graphics::shader_graph_nodes