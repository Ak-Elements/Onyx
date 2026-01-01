#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    template <typename T, CompileTimeString TypeIdString>
    class GetParticleVariableNode : public NodeGraph::FixedPinNode_1_In_1_Out<ShaderGraphNode, String, T>
    {
        using Super = NodeGraph::FixedPinNode_1_In_1_Out<ShaderGraphNode, String, T>;
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override
        {
            generator.AppendCode(Format::Format("{} pin_{:x} = particle.{}; \n",
                ShaderGenerator::GetTypeAsShaderTypeString<T>(),
                Super::GetOutputPin().GetGlobalId().Get(),
                context.GetPinData<typename Super::InPin>()));
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin::LocalId: return "Name";
            case Super::OutPin::LocalId: return "Value";
            }

            return "";
#endif
        }
    };

    using GetParticleVariableF32 = GetParticleVariableNode<onyxF32, "Onyx::Graphics::ShaderGraph::GetParticleVariableF32">;
    using GetParticleVariableVector3f32 = GetParticleVariableNode<Vector3f32, "Onyx::Graphics::ShaderGraph::GetParticleVariableVector3f32">;
}
