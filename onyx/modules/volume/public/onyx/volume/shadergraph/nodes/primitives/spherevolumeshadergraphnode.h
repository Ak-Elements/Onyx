#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Volume
{
    class SphereVolumeShaderGraphNode : public NodeGraph::FixedPinNode_2_In_2_Out<Graphics::ShaderGraphNode, Vector3f32, onyxF32, Vector3f32, onyxF32>
    {
    private:
        using Super = NodeGraph::FixedPinNode_2_In_2_Out<Graphics::ShaderGraphNode, Vector3f32, onyxF32, Vector3f32, onyxF32>;
    public:
        SphereVolumeShaderGraphNode();
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::SphereVolumeShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const override;
        
#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}