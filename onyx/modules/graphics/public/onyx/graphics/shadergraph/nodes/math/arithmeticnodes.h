#pragma once
#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace Onyx::Graphics
{
    class ShaderGraphNode;
}

namespace Onyx::NodeGraph
{
    template <typename ScalarT, CompileTimeString TypeIdString>
    class AddNode<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
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

        void OnUIDrawNodeBackground() override
        {
           // DrawPlusIconBackground(Super::GetId());
        }

        StringView GetPinName(StringId32 /*pinId*/) const override
        {
            // this node should not show any labels
            return "";
        }
#endif
    };

    template <typename ScalarT, CompileTimeString TypeIdString>
    class SubtractNode<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
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

        void OnUIDrawNodeBackground() override
        {
           // DrawMinusIconBackground(Super::GetId());
        }

        StringView GetPinName(StringId32 /*pinId*/) const override
        {
            // this node should not show any labels
            return "";
        }
#endif
    };

    template <typename ScalarT, CompileTimeString TypeIdString>
    class MultiplyNode<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 * pin1;
        }

#if ONYX_IS_EDITOR
    private:
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

        void OnUIDrawNodeBackground() override
        {
            //DrawMultiplyIconBackground(Super::GetId());
        }

        StringView GetPinName(StringId32 /*pinId*/) const override
        {
            // this node should not show any labels
            return "";
        }
#endif
    };

    template <typename ScalarT, CompileTimeString TypeIdString>
    class DivisionNode<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 / pin1;
        }

#if ONYX_IS_EDITOR
    private:
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

        void OnUIDrawNodeBackground() override
        {
            //DrawDivisionIconBackground(Super::GetId());
        }

        StringView GetPinName(StringId32 /*pinId*/) const override
        {
            // this node should not show any labels
            return "";
        }
#endif
    };

    template <typename DataT, CompileTimeString TypeIdString>
    class AbsoluteNode<Graphics::ShaderGraphNode, DataT, TypeIdString> : public FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, DataT, DataT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
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
    private:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin& inputPin = Super::GetInputPin();
            generator.AppendCode("// Absolute node \n");
            generator.AppendCode(Format::Format("{} pin_{:x} = abs({}); \n",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                inputPin.IsConnected() ? Format::Format("pin_{:x}", inputPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>())));
        }

        void OnUIDrawNodeBackground() override
        {
           // DrawAbsoluteIconBackground(Super::GetId());
        }

        StringView GetPinName(StringId32 /*pinId*/) const override
        {
            // this node should not show any labels
            return "";
        }
#endif
    };
}

namespace Onyx::Graphics::ShaderGraphNodes
{
    // Addition
    using AddNodeF32 = NodeGraph::AddNode<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::AdditionF32">;
    using AddNodeF64 = NodeGraph::AddNode<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::AdditionF64">;
    using AddNodeS32 = NodeGraph::AddNode<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::AdditionS32">;
    using AddNodeS64 = NodeGraph::AddNode<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::AdditionS64">;
    using AddNodeVector2f32 = NodeGraph::AddNode<ShaderGraphNode, Vector2f32, "Onyx::Graphics::ShaderGraph::AdditionVector2f32">;
    using AddNodeVector2f64 = NodeGraph::AddNode<ShaderGraphNode, Vector2f64, "Onyx::Graphics::ShaderGraph::AdditionVector2f64">;
    using AddNodeVector2s32 = NodeGraph::AddNode<ShaderGraphNode, Vector2s32, "Onyx::Graphics::ShaderGraph::AdditionVector2s32">;
    using AddNodeVector2s64 = NodeGraph::AddNode<ShaderGraphNode, Vector2s64, "Onyx::Graphics::ShaderGraph::AdditionVector2s64">;
    using AddNodeVector3f32 = NodeGraph::AddNode<ShaderGraphNode, Vector3f32, "Onyx::Graphics::ShaderGraph::AdditionVector3f32">;
    using AddNodeVector3f64 = NodeGraph::AddNode<ShaderGraphNode, Vector3f64, "Onyx::Graphics::ShaderGraph::AdditionVector3f64">;
    using AddNodeVector3s32 = NodeGraph::AddNode<ShaderGraphNode, Vector3s32, "Onyx::Graphics::ShaderGraph::AdditionVector3s32">;
    using AddNodeVector3s64 = NodeGraph::AddNode<ShaderGraphNode, Vector3s64, "Onyx::Graphics::ShaderGraph::AdditionVector3s64">;
    using AddNodeVector4f32 = NodeGraph::AddNode<ShaderGraphNode, Vector4f32, "Onyx::Graphics::ShaderGraph::AdditionVector4f32">;
    using AddNodeVector4f64 = NodeGraph::AddNode<ShaderGraphNode, Vector4f64, "Onyx::Graphics::ShaderGraph::AdditionVector4f64">;
    using AddNodeVector4s32 = NodeGraph::AddNode<ShaderGraphNode, Vector4s32, "Onyx::Graphics::ShaderGraph::AdditionVector4s32">;
    using AddNodeVector4s64 = NodeGraph::AddNode<ShaderGraphNode, Vector4s64, "Onyx::Graphics::ShaderGraph::AdditionVector4s64">;

    // Subtraction
    using SubtractNodeF32 = NodeGraph::SubtractNode<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::SubtractF32">;
    using SubtractNodeF64 = NodeGraph::SubtractNode<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::SubtractF64">;
    using SubtractNodeS32 = NodeGraph::SubtractNode<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::SubtractS32">;
    using SubtractNodeS64 = NodeGraph::SubtractNode<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::SubtractS64">;
    using SubtractNodeVector2f32 = NodeGraph::SubtractNode<ShaderGraphNode, Vector2f32, "Onyx::Graphics::ShaderGraph::SubtractVector2f32">;
    using SubtractNodeVector2f64 = NodeGraph::SubtractNode<ShaderGraphNode, Vector2f64, "Onyx::Graphics::ShaderGraph::SubtractVector2f64">;
    using SubtractNodeVector2s32 = NodeGraph::SubtractNode<ShaderGraphNode, Vector2s32, "Onyx::Graphics::ShaderGraph::SubtractVector2s32">;
    using SubtractNodeVector2s64 = NodeGraph::SubtractNode<ShaderGraphNode, Vector2s64, "Onyx::Graphics::ShaderGraph::SubtractVector2s64">;
    using SubtractNodeVector3f32 = NodeGraph::SubtractNode<ShaderGraphNode, Vector3f32, "Onyx::Graphics::ShaderGraph::SubtractVector3f32">;
    using SubtractNodeVector3f64 = NodeGraph::SubtractNode<ShaderGraphNode, Vector3f64, "Onyx::Graphics::ShaderGraph::SubtractVector3f64">;
    using SubtractNodeVector3s32 = NodeGraph::SubtractNode<ShaderGraphNode, Vector3s32, "Onyx::Graphics::ShaderGraph::SubtractVector3s32">;
    using SubtractNodeVector3s64 = NodeGraph::SubtractNode<ShaderGraphNode, Vector3s64, "Onyx::Graphics::ShaderGraph::SubtractVector3s64">;
    using SubtractNodeVector4f32 = NodeGraph::SubtractNode<ShaderGraphNode, Vector4f32, "Onyx::Graphics::ShaderGraph::SubtractVector4f32">;
    using SubtractNodeVector4f64 = NodeGraph::SubtractNode<ShaderGraphNode, Vector4f64, "Onyx::Graphics::ShaderGraph::SubtractVector4f64">;
    using SubtractNodeVector4s32 = NodeGraph::SubtractNode<ShaderGraphNode, Vector4s32, "Onyx::Graphics::ShaderGraph::SubtractVector4s32">;
    using SubtractNodeVector4s64 = NodeGraph::SubtractNode<ShaderGraphNode, Vector4s64, "Onyx::Graphics::ShaderGraph::SubtractVector4s64">;

    // Multiplication
    using MultiplyNodeF32 = NodeGraph::MultiplyNode<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::MultiplyF32">;
    using MultiplyNodeF64 = NodeGraph::MultiplyNode<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::MultiplyF64">;
    using MultiplyNodeS32 = NodeGraph::MultiplyNode<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::MultiplyS32">;
    using MultiplyNodeS64 = NodeGraph::MultiplyNode<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::MultiplyS64">;

    // Division 
    using DivisionNodeF32 = NodeGraph::DivisionNode<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::DivideF32">;
    using DivisionNodeF64 = NodeGraph::DivisionNode<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::DivideF64">;
    using DivisionNodeS32 = NodeGraph::DivisionNode<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::DivideS32">;
    using DivisionNodeS64 = NodeGraph::DivisionNode<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::DivideS64">;

    using AbsoluteNodeF32 = NodeGraph::AbsoluteNode<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::AbsoluteF32">;
    using AbsoluteNodeF64 = NodeGraph::AbsoluteNode<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::AbsoluteF64">;
    using AbsoluteNodeS32 = NodeGraph::AbsoluteNode<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::AbsoluteS32">;
    using AbsoluteNodeS64 = NodeGraph::AbsoluteNode<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::AbsoluteS64">;
    using AbsoluteNodeVector2f32 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector2f32, "Onyx::Graphics::ShaderGraph::AbsoluteVector2f32">;
    using AbsoluteNodeVector2f64 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector2f64, "Onyx::Graphics::ShaderGraph::AbsoluteVector2f64">;
    using AbsoluteNodeVector2s32 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector2s32, "Onyx::Graphics::ShaderGraph::AbsoluteVector2s32">;
    using AbsoluteNodeVector2s64 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector2s64, "Onyx::Graphics::ShaderGraph::AbsoluteVector2s64">;
    using AbsoluteNodeVector3f32 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector3f32, "Onyx::Graphics::ShaderGraph::AbsoluteVector3f32">;
    using AbsoluteNodeVector3f64 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector3f64, "Onyx::Graphics::ShaderGraph::AbsoluteVector3f64">;
    using AbsoluteNodeVector3s32 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector3s32, "Onyx::Graphics::ShaderGraph::AbsoluteVector3s32">;
    using AbsoluteNodeVector3s64 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector3s64, "Onyx::Graphics::ShaderGraph::AbsoluteVector3s64">;
    using AbsoluteNodeVector4f32 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector4f32, "Onyx::Graphics::ShaderGraph::AbsoluteVector4f32">;
    using AbsoluteNodeVector4f64 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector4f64, "Onyx::Graphics::ShaderGraph::AbsoluteVector4f64">;
    using AbsoluteNodeVector4s32 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector4s32, "Onyx::Graphics::ShaderGraph::AbsoluteVector4s32">;
    using AbsoluteNodeVector4s64 = NodeGraph::AbsoluteNode<ShaderGraphNode, Vector4s64, "Onyx::Graphics::ShaderGraph::AbsoluteVector4s64">;
}
