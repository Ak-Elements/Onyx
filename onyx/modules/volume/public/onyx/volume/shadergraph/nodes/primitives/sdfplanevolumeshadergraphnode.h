#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/volume/source/sdfsample.h>

namespace onyx::volume {
class SdfPlaneVolumeShaderGraphNode
    : public node_graph::FixedPinNode2In1Out< graphics::ShaderGraphNode, Vector3f32, float32, SdfSample > {
  private:
    using Super = node_graph::FixedPinNode2In1Out< graphics::ShaderGraphNode, Vector3f32, float32, SdfSample >;

  public:
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfPlaneVolumeShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    SdfPlaneVolumeShaderGraphNode() = default;

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
