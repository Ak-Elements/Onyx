#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>

#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::NodeGraph
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

    template <typename FactoryType, CompileTimeString SerializedNameSpaceId> 
    void RegisterArithmeticNodes()
    {
        using FactoryNodeTypeT = typename FactoryType::NodeTypeT;

        // Addition
        FactoryType::template Register<AddNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::AdditionF32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::AdditionF64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::AdditionS32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::AdditionS64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector2f32, SerializedNameSpaceId + "::AdditionVector2f32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector2f64, SerializedNameSpaceId + "::AdditionVector2f64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector2s32, SerializedNameSpaceId + "::AdditionVector2s32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector2s64, SerializedNameSpaceId + "::AdditionVector2s64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector3f32, SerializedNameSpaceId + "::AdditionVector3f32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector3f64, SerializedNameSpaceId + "::AdditionVector3f64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector3s32, SerializedNameSpaceId + "::AdditionVector3s32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector3s64, SerializedNameSpaceId + "::AdditionVector3s64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector4f32, SerializedNameSpaceId + "::AdditionVector4f32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector4f64, SerializedNameSpaceId + "::AdditionVector4f64">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector4s32, SerializedNameSpaceId + "::AdditionVector4s32">>();
        FactoryType::template Register<AddNode<FactoryNodeTypeT, Vector4s64, SerializedNameSpaceId + "::AdditionVector4s64">>();

        // Subtraction
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::SubtractF32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::SubtractF64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::SubtractS32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::SubtractS64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector2f32, SerializedNameSpaceId + "::SubtractVector2f32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector2f64, SerializedNameSpaceId + "::SubtractVector2f64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector2s32, SerializedNameSpaceId + "::SubtractVector2s32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector2s64, SerializedNameSpaceId + "::SubtractVector2s64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector3f32, SerializedNameSpaceId + "::SubtractVector3f32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector3f64, SerializedNameSpaceId + "::SubtractVector3f64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector3s32, SerializedNameSpaceId + "::SubtractVector3s32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector3s64, SerializedNameSpaceId + "::SubtractVector3s64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector4f32, SerializedNameSpaceId + "::SubtractVector4f32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector4f64, SerializedNameSpaceId + "::SubtractVector4f64">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector4s32, SerializedNameSpaceId + "::SubtractVector4s32">>();
        FactoryType::template Register<SubtractNode<FactoryNodeTypeT, Vector4s64, SerializedNameSpaceId + "::SubtractVector4s64">>();

        // Multiplication
        FactoryType::template Register<MultiplyNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::MultiplyF32">>();
        FactoryType::template Register<MultiplyNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::MultiplyF64">>();
        FactoryType::template Register<MultiplyNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::MultiplyS32">>();
        FactoryType::template Register<MultiplyNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::MultiplyS64">>();

        // Division 
        FactoryType::template Register<DivisionNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::DivideF32">>();
        FactoryType::template Register<DivisionNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::DivideF64">>();
        FactoryType::template Register<DivisionNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::DivideS32">>();
        FactoryType::template Register<DivisionNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::DivideS64">>();

        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::AbsoluteF32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::AbsoluteF64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::AbsoluteS32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::AbsoluteS64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector2f32, SerializedNameSpaceId + "::AbsoluteVector2f32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector2f64, SerializedNameSpaceId + "::AbsoluteVector2f64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector2s32, SerializedNameSpaceId + "::AbsoluteVector2s32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector2s64, SerializedNameSpaceId + "::AbsoluteVector2s64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector3f32, SerializedNameSpaceId + "::AbsoluteVector3f32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector3f64, SerializedNameSpaceId + "::AbsoluteVector3f64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector3s32, SerializedNameSpaceId + "::AbsoluteVector3s32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector3s64, SerializedNameSpaceId + "::AbsoluteVector3s64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector4f32, SerializedNameSpaceId + "::AbsoluteVector4f32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector4f64, SerializedNameSpaceId + "::AbsoluteVector4f64">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector4s32, SerializedNameSpaceId + "::AbsoluteVector4s32">>();
        FactoryType::template Register<AbsoluteNode<FactoryNodeTypeT, Vector4s64, SerializedNameSpaceId + "::AbsoluteVector4s64">>();
    }
}
