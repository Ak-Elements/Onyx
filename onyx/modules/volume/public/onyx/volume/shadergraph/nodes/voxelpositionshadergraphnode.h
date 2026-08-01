#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>

namespace onyx::volume {
class GetVoxelPositionShaderGraphNode : public node_graph::FixedPinNode1Out< graphics::ShaderGraphNode, Vector3f32 > {
  public:
    GetVoxelPositionShaderGraphNode();
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::GetVoxelPositionShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    void onUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::volume
