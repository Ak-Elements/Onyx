#pragma once
#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Graphics
{
    class ShaderGraphNode;
}

namespace Onyx::NodeGraph
{
    template <typename ScalarT>
    class AddNode<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 + pin1;
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin0::LocalId: return "a";
            case Super::InPin1::LocalId: return "b";
            case Super::OutPin::LocalId: return "Result";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif

#if ONYX_IS_EDITOR
    private:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            generator.AppendCode("// Add node \n");
            generator.AppendCode(Format::Format("{} pin_{:x} = ", Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(), Super::GetOutputPin().GetGlobalId().Get()));

            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            generator.AppendCode(inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()));
            generator.AppendCode(" + ");
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
            generator.AppendCode(inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>()));
            generator.AppendCode("; \n");

        }
#endif
    };

    template <typename ScalarT>
    class SubtractNode<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 - pin1;
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin0::LocalId: return "a";
            case Super::InPin1::LocalId: return "b";
            case Super::OutPin::LocalId: return "Result";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif

#if ONYX_IS_EDITOR
    private:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            generator.AppendCode("// Subtract node \n");
            generator.AppendCode(Format::Format("{} pin_{:x} = ", Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(), Super::GetOutputPin().GetGlobalId().Get()));

            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            generator.AppendCode(inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()));
            generator.AppendCode(" - ");
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
            generator.AppendCode(inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>()));
            generator.AppendCode("; \n");
        }
#endif
    };

    template <typename ScalarT>
    class MultiplyNode<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 * pin1;
        }

#if ONYX_IS_EDITOR
    public:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            generator.AppendCode("// Multiply node \n");
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
            case Super::InPin0::LocalId: return "a";
            case Super::InPin1::LocalId: return "b";
            case Super::OutPin::LocalId: return "Result";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename ScalarT>
    class DivisionNode<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 / pin1;
        }

#if ONYX_IS_EDITOR
    public:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            generator.AppendCode("// Division node \n");
            generator.AppendCode(Format::Format("{} pin_{:x} = ", Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(), Super::GetOutputPin().GetGlobalId().Get()));

            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            generator.AppendCode(inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()));
            generator.AppendCode(" / ");
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
            generator.AppendCode(inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>()));
            generator.AppendCode("; \n");
        }

    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin0::LocalId: return "a";
            case Super::InPin1::LocalId: return "b";
            case Super::OutPin::LocalId: return "Result";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename DataT>
    class AbsoluteNode<Graphics::ShaderGraphNode, DataT> : public FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, DataT, DataT>
    {
        using Super = FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, DataT, DataT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            DataT inPinValue = context.GetPinData<typename Super::InPin>();
            DataT& outPinValue = context.GetPinData<typename Super::OutPin>();

            if constexpr (is_specialization_of_v<Vector4, DataT>)
            {
                outPinValue = DataT(std::abs(inPinValue[0]), std::abs(inPinValue[1]), std::abs(inPinValue[2]), std::abs(inPinValue[3]));
            }
            else if constexpr (is_specialization_of_v<Vector3, DataT>)
            {
                outPinValue = DataT(std::abs(inPinValue[0]), std::abs(inPinValue[1]), std::abs(inPinValue[2]));
            }
            else if constexpr (is_specialization_of_v<Vector2, DataT>)
            {
                outPinValue = DataT(std::abs(inPinValue[0]), std::abs(inPinValue[1]));
            }
            else if constexpr (std::is_integral_v<DataT> || std::is_floating_point_v<DataT>)
            {
                outPinValue = std::abs(inPinValue);
            }
        }

#if ONYX_IS_EDITOR
    public:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin& inputPin = Super::GetInputPin();
            generator.AppendCode("// Absolute node \n");
            generator.AppendCode(Format::Format("{} pin_{:x} = abs({}); \n",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                inputPin.IsConnected() ? Format::Format("pin_{:x}", inputPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>())));
        }

    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin::LocalId: return "In";
                case Super::OutPin::LocalId: return "| In |";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}
