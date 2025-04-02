#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics
{
    class GetWorldPositionNode : public NodeGraph::FixedPinNode_1_Out<ShaderGraphNode, Vector4f>
    {
        using Super = FixedPinNode_1_Out<ShaderGraphNode, Vector4f>;

    public:
        void DoGenerateShader(const NodeGraph::ExecutionContext&, ShaderGenerator&) const override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override;
#endif
    };
}
