#pragma once
#include <onyx/nodegraph/nodes/math/geometricnodes.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::NodeGraph
{
    template <template <typename ScalarT> class VectorT, typename ScalarT, CompileTimeString TypeIdString>
    class ScaleVectorNode<Graphics::ShaderGraphNode, VectorT, ScalarT, TypeIdString> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, VectorT<ScalarT>, ScalarT, VectorT<ScalarT>>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, VectorT<ScalarT>, ScalarT, VectorT<ScalarT>>;

        void OnUpdate(ExecutionContext& context) const override
        {
            VectorT<ScalarT> pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            VectorT<ScalarT>& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 * pin1;
        }

#if ONYX_IS_EDITOR
    private:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            generator.AppendCode("// Scale vector node \n");
            generator.AppendCode(Format::Format("{} pin_{:x} = ", Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(), Super::GetOutputPin().GetGlobalId().Get()));

            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            generator.AppendCode(inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()));
            generator.AppendCode(" * ");
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
            generator.AppendCode(inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>()));
            generator.AppendCode("; \n");
        }
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin0::LocalId: return "Vector";
            case Super::InPin1::LocalId: return "Scale";
            case Super::OutPin::LocalId: return "Result";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}
