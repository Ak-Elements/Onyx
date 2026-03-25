#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>

namespace onyx::volume {
class SdfCubeVolumeShaderGraphNode
    : public node_graph::
          FixedPinNode_2_In_2_Out< graphics::ShaderGraphNode, Vector3f32, Vector3f32, float32, Vector3f32 > {
  private:
    using Super = node_graph::
        FixedPinNode_2_In_2_Out< graphics::ShaderGraphNode, Vector3f32, Vector3f32, float32, Vector3f32 >;

  public:
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfCubeVolumeShaderGraphNode";
    StringId32 GetTypeId() const override { return TypeId; }

    SdfCubeVolumeShaderGraphNode() = default;

    void OnUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void DoGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    StringView GetPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::volume