#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace onyx::volume
{
    class SdfSphereVolumeShaderGraphNode : public node_graph::FixedPinNode_2_In_2_Out<graphics::ShaderGraphNode, Vector3f32, onyxF32, onyxF32, Vector3f32>
    {
    private:
        using Super = node_graph::FixedPinNode_2_In_2_Out<graphics::ShaderGraphNode, Vector3f32, onyxF32, onyxF32, Vector3f32>;
    public:
        static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfSphereVolumeShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfSphereVolumeShaderGraphNode() = default;

        void OnUpdate(node_graph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const override;
        
#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}