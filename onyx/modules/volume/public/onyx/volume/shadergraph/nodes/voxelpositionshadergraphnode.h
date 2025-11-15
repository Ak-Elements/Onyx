#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Volume
{
    class GetVoxelPositionShaderGraphNode : public NodeGraph::FixedPinNode_1_Out<Graphics::ShaderGraphNode, Vector3f32>
    {
    public:
        GetVoxelPositionShaderGraphNode();
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::GetVoxelPositionShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}
