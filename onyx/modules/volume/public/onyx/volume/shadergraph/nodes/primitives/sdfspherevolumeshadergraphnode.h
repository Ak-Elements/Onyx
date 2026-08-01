#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>

namespace onyx::volume {
class SdfSphereVolumeShaderGraphNode
    : public node_graph::FixedPinNode2In2Out< graphics::ShaderGraphNode, Vector3f32, float32, float32, Vector3f32 > {
  private:
    using Super = node_graph::
        FixedPinNode2In2Out< graphics::ShaderGraphNode, Vector3f32, float32, float32, Vector3f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfSphereVolumeShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    SdfSphereVolumeShaderGraphNode() = default;

    void onUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::volume
