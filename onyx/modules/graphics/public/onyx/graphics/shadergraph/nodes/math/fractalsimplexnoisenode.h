#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in2out.h>

namespace onyx::ui {
template < typename T >
struct PropertyInspector;
}

namespace onyx::graphics::shader_graph_nodes {
class FractalSimplexNoise2DNode
    : public node_graph::FixedPinNode1In2Out< ShaderGraphNode, Vector2f32, float32, Vector2f32 > {
  private:
    friend struct onyx::ui::PropertyInspector< FractalSimplexNoise2DNode >;

    using Super = node_graph::FixedPinNode1In2Out< ShaderGraphNode, Vector2f32, float32, Vector2f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::FractalSimplexNoise2DShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    FractalSimplexNoise2DNode() = default;

    void onUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif

  private:
    struct Octave {
        float32 Frequency;
        float32 Amplitude;
    };

    DynamicArray< Octave > m_octaves;
};

} // namespace onyx::graphics::shader_graph_nodes
