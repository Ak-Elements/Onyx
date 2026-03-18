#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>

#include <onyx/nodegraph/nodes/node.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::node_graph
{
    template <typename NodeType, typename ScalarT, CompileTimeString TypeIdString>
    class AddNode : public FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 + pin1;
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename NodeType, typename ScalarT, CompileTimeString TypeIdString>
    class SubtractNode : public FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 - pin1;
        }

#if ONYX_IS_EDITOR
    private:

        StringView GetPinName(StringId32 pinId) const override
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

    template <typename NodeType, typename ScalarT, CompileTimeString TypeIdString>
    class MultiplyNode : public FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 * pin1;
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename NodeType, typename ScalarT, CompileTimeString TypeIdString>
    class DivisionNode : public FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr bool HasAliases = true;
        static constexpr bool ShowNodeName = false;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            ScalarT& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 / pin1;
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename NodeType, typename DataT, CompileTimeString TypeIdString>
    class AbsoluteNode : public FixedPinNode_1_In_1_Out<NodeType, DataT, DataT>
    {
    public:
        static constexpr bool HasAliases = true;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_1_In_1_Out<NodeType, DataT, DataT>;

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
        StringView GetPinName(StringId32 pinId) const override
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

    namespace Nodes
    {
        // Addition
        using AddNodeF32 = AddNode<Node, onyxF32, "onyx::NodeGraph::Nodes::AdditionF32">;
        using AddNodeF64 = AddNode<Node, onyxF64, "onyx::NodeGraph::Nodes::AdditionF64">;
        using AddNodeS32 = AddNode<Node, onyxS32, "onyx::NodeGraph::Nodes::AdditionS32">;
        using AddNodeS64 = AddNode<Node, onyxS64, "onyx::NodeGraph::Nodes::AdditionS64">;
        using AddNodeVector2f32 = AddNode<Node, Vector2f32, "onyx::NodeGraph::Nodes::AdditionVector2f32">;
        using AddNodeVector2f64 = AddNode<Node, Vector2f64, "onyx::NodeGraph::Nodes::AdditionVector2f64">;
        using AddNodeVector2s32 = AddNode<Node, Vector2s32, "onyx::NodeGraph::Nodes::AdditionVector2s32">;
        using AddNodeVector2s64 = AddNode<Node, Vector2s64, "onyx::NodeGraph::Nodes::AdditionVector2s64">;
        using AddNodeVector3f32 = AddNode<Node, Vector3f32, "onyx::NodeGraph::Nodes::AdditionVector3f32">;
        using AddNodeVector3f64 = AddNode<Node, Vector3f64, "onyx::NodeGraph::Nodes::AdditionVector3f64">;
        using AddNodeVector3s32 = AddNode<Node, Vector3s32, "onyx::NodeGraph::Nodes::AdditionVector3s32">;
        using AddNodeVector3s64 = AddNode<Node, Vector3s64, "onyx::NodeGraph::Nodes::AdditionVector3s64">;
        using AddNodeVector4f32 = AddNode<Node, Vector4f32, "onyx::NodeGraph::Nodes::AdditionVector4f32">;
        using AddNodeVector4f64 = AddNode<Node, Vector4f64, "onyx::NodeGraph::Nodes::AdditionVector4f64">;
        using AddNodeVector4s32 = AddNode<Node, Vector4s32, "onyx::NodeGraph::Nodes::AdditionVector4s32">;
        using AddNodeVector4s64 = AddNode<Node, Vector4s64, "onyx::NodeGraph::Nodes::AdditionVector4s64">;

        //// Subtraction
        using SubtractNodeF32 = SubtractNode<Node, onyxF32, "onyx::NodeGraph::Nodes::SubtractF32">;
        using SubtractNodeF64 = SubtractNode<Node, onyxF64, "onyx::NodeGraph::Nodes::SubtractF64">;
        using SubtractNodeS32 = SubtractNode<Node, onyxS32, "onyx::NodeGraph::Nodes::SubtractS32">;
        using SubtractNodeS64 = SubtractNode<Node, onyxS64, "onyx::NodeGraph::Nodes::SubtractS64">;
        using SubtractNodeVector2f32 = SubtractNode<Node, Vector2f32, "onyx::NodeGraph::Nodes::SubtractVector2f32">;
        using SubtractNodeVector2f64 = SubtractNode<Node, Vector2f64, "onyx::NodeGraph::Nodes::SubtractVector2f64">;
        using SubtractNodeVector2s32 = SubtractNode<Node, Vector2s32, "onyx::NodeGraph::Nodes::SubtractVector2s32">;
        using SubtractNodeVector2s64 = SubtractNode<Node, Vector2s64, "onyx::NodeGraph::Nodes::SubtractVector2s64">;
        using SubtractNodeVector3f32 = SubtractNode<Node, Vector3f32, "onyx::NodeGraph::Nodes::SubtractVector3f32">;
        using SubtractNodeVector3f64 = SubtractNode<Node, Vector3f64, "onyx::NodeGraph::Nodes::SubtractVector3f64">;
        using SubtractNodeVector3s32 = SubtractNode<Node, Vector3s32, "onyx::NodeGraph::Nodes::SubtractVector3s32">;
        using SubtractNodeVector3s64 = SubtractNode<Node, Vector3s64, "onyx::NodeGraph::Nodes::SubtractVector3s64">;
        using SubtractNodeVector4f32 = SubtractNode<Node, Vector4f32, "onyx::NodeGraph::Nodes::SubtractVector4f32">;
        using SubtractNodeVector4f64 = SubtractNode<Node, Vector4f64, "onyx::NodeGraph::Nodes::SubtractVector4f64">;
        using SubtractNodeVector4s32 = SubtractNode<Node, Vector4s32, "onyx::NodeGraph::Nodes::SubtractVector4s32">;
        using SubtractNodeVector4s64 = SubtractNode<Node, Vector4s64, "onyx::NodeGraph::Nodes::SubtractVector4s64">;

        //// Multiplication
        using MultiplyNodeF32 = MultiplyNode<Node, onyxF32, "onyx::NodeGraph::Nodes::MultiplyF32">;
        using MultiplyNodeF64 = MultiplyNode<Node, onyxF64, "onyx::NodeGraph::Nodes::MultiplyF64">;
        using MultiplyNodeS32 = MultiplyNode<Node, onyxS32, "onyx::NodeGraph::Nodes::MultiplyS32">;
        using MultiplyNodeS64 = MultiplyNode<Node, onyxS64, "onyx::NodeGraph::Nodes::MultiplyS64">;

        //// Division 
        using DivisionNodeF32 = DivisionNode<Node, onyxF32, "onyx::NodeGraph::Nodes::DivideF32">;
        using DivisionNodeF64 = DivisionNode<Node, onyxF64, "onyx::NodeGraph::Nodes::DivideF64">;
        using DivisionNodeS32 = DivisionNode<Node, onyxS32, "onyx::NodeGraph::Nodes::DivideS32">;
        using DivisionNodeS64 = DivisionNode<Node, onyxS64, "onyx::NodeGraph::Nodes::DivideS64">;

        using AbsoluteNodeF32 = AbsoluteNode<Node, onyxF32, "onyx::NodeGraph::Nodes::AbsoluteF32">;
        using AbsoluteNodeF64 = AbsoluteNode<Node, onyxF64, "onyx::NodeGraph::Nodes::AbsoluteF64">;
        using AbsoluteNodeS32 = AbsoluteNode<Node, onyxS32, "onyx::NodeGraph::Nodes::AbsoluteS32">;
        using AbsoluteNodeS64 = AbsoluteNode<Node, onyxS64, "onyx::NodeGraph::Nodes::AbsoluteS64">;
        using AbsoluteNodeVector2f32 = AbsoluteNode<Node, Vector2f32, "onyx::NodeGraph::Nodes::AbsoluteVector2f32">;
        using AbsoluteNodeVector2f64 = AbsoluteNode<Node, Vector2f64, "onyx::NodeGraph::Nodes::AbsoluteVector2f64">;
        using AbsoluteNodeVector2s32 = AbsoluteNode<Node, Vector2s32, "onyx::NodeGraph::Nodes::AbsoluteVector2s32">;
        using AbsoluteNodeVector2s64 = AbsoluteNode<Node, Vector2s64, "onyx::NodeGraph::Nodes::AbsoluteVector2s64">;
        using AbsoluteNodeVector3f32 = AbsoluteNode<Node, Vector3f32, "onyx::NodeGraph::Nodes::AbsoluteVector3f32">;
        using AbsoluteNodeVector3f64 = AbsoluteNode<Node, Vector3f64, "onyx::NodeGraph::Nodes::AbsoluteVector3f64">;
        using AbsoluteNodeVector3s32 = AbsoluteNode<Node, Vector3s32, "onyx::NodeGraph::Nodes::AbsoluteVector3s32">;
        using AbsoluteNodeVector3s64 = AbsoluteNode<Node, Vector3s64, "onyx::NodeGraph::Nodes::AbsoluteVector3s64">;
        using AbsoluteNodeVector4f32 = AbsoluteNode<Node, Vector4f32, "onyx::NodeGraph::Nodes::AbsoluteVector4f32">;
        using AbsoluteNodeVector4f64 = AbsoluteNode<Node, Vector4f64, "onyx::NodeGraph::Nodes::AbsoluteVector4f64">;
        using AbsoluteNodeVector4s32 = AbsoluteNode<Node, Vector4s32, "onyx::NodeGraph::Nodes::AbsoluteVector4s32">;
        using AbsoluteNodeVector4s64 = AbsoluteNode<Node, Vector4s64, "onyx::NodeGraph::Nodes::AbsoluteVector4s64">;
    }
}
