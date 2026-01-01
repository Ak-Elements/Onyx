#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    // TODO: 2 out - return milliseconds or seconds depending on whats connected
    // Or dropdown selecting the unit type?
    class GetDeltaTimeNode : public NodeGraph::FixedPinNode_1_Out<ShaderGraphNode, onyxF32>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::GetDeltaTime";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext&, ShaderGenerator&) const override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}
