#pragma once

#include <onyx/nodegraph/node.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/geometry/common.h>

namespace Onyx::Graphics
{
    class FragmentShaderOutNode : public NodeGraph::FixedPinNode_1_In<ShaderGraphNode, Vector4f>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::FragmentShaderOut";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };

    class PBRMaterialShaderOutNode : public NodeGraph::FixedPinNode<ShaderGraphNode, 4, 0>
    {
    private:
        using AlbedoInPin = NodeGraph::Pin<Vector3f, "Albedo">;
        using NormalInPin = NodeGraph::Pin<Vector3f, "Normal">;
        using MetalnessInPin = NodeGraph::Pin<onyxF32, "Metalness">;
        using RoughnessInPin = NodeGraph::Pin<onyxF32, "Roughness">;

    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::PBRMaterialShaderOut";
       StringId32 GetTypeId() const override { return TypeId; }

        PBRMaterialShaderOutNode();
    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;
    };
}
