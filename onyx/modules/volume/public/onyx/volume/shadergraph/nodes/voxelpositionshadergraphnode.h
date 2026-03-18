#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace onyx::volume
{
    class GetVoxelPositionShaderGraphNode : public node_graph::FixedPinNode_1_Out<graphics::ShaderGraphNode, Vector3f32>
    {
    public:
        GetVoxelPositionShaderGraphNode();
        static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::GetVoxelPositionShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        void OnUpdate(node_graph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}
