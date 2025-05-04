#pragma once
#include <onyx/nodegraph/node.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::NodeGraph
{
    template <typename NodeType, typename ScalarT, CompileTimeString TypeIdString>
    class AddNode : public FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT>
    {
    public:
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
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::AdditionF32">>("Math/Float/Float + Float");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::AdditionF64">>("Math/Double/Double + Double");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::AdditionS32">>("Math/Integer/Integer + Integer");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::AdditionS64">>("Math/Integer64/Integer64 + Integer64");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector2f, SerializedNameSpaceId + "::AdditionVector2f32">>("Math/Vector2f/Vector2f + Vector2f");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector2d, SerializedNameSpaceId + "::AdditionVector2f64">>("Math/Vector2d/Vector2d + Vector2d");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector2s32, SerializedNameSpaceId + "::AdditionVector2s32">>("Math/Vector2s32/Vector2s32 + Vector2s32");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector2s64, SerializedNameSpaceId + "::AdditionVector2s64">>("Math/Vector2s64/Vector2s64 + Vector2s64");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector3f, SerializedNameSpaceId + "::AdditionVector3f32">>("Math/Vector3f/Vector3f + Vector3f");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector3d, SerializedNameSpaceId + "::AdditionVector3f64">>("Math/Vector3d/Vector3d + Vector3d");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector3s32, SerializedNameSpaceId + "::AdditionVector3s32">>("Math/Vector3s32/Vector3s32 + Vector3s32");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector3s64, SerializedNameSpaceId + "::AdditionVector3s64">>("Math/Vector3s64/Vector3s64 + Vector3s64");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector4f, SerializedNameSpaceId + "::AdditionVector4f32">>("Math/Vector4f/Vector4f + Vector4f");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector4d, SerializedNameSpaceId + "::AdditionVector4f64">>("Math/Vector4d/Vector4d + Vector4d");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector4s32, SerializedNameSpaceId + "::AdditionVector4s32">>("Math/Vector4s32/Vector4s32 + Vector4s32");
        FactoryType::template RegisterNode<AddNode<FactoryNodeTypeT, Vector4s64, SerializedNameSpaceId + "::AdditionVector4s64">>("Math/Vector4s64/Vector4s64 + Vector4s64");

        // Subtraction
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::SubtractFloat">>("Math/Float/Float - Float");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::SubtractDouble">>("Math/Double/Double - Double");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::SubtractInteger">>("Math/Integer/Integer - Integer");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::SubtractInteger64">>("Math/Integer64/Integer64 - Integer64");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector2f, SerializedNameSpaceId + "::SubtractVector2f32">>("Math/Vector2f/Vector2f - Vector2f");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector2d, SerializedNameSpaceId + "::SubtractVector2f64">>("Math/Vector2d/Vector2d - Vector2d");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector2s32, SerializedNameSpaceId + "::SubtractVector2s32">>("Math/Vector2s32/Vector2s32 - Vector2s32");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector2s64, SerializedNameSpaceId + "::SubtractVector2s64">>("Math/Vector2s64/Vector2s64 - Vector2s64");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector3f, SerializedNameSpaceId + "::SubtractVector3f32">>("Math/Vector3f/Vector3f - Vector3f");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector3d, SerializedNameSpaceId + "::SubtractVector3f64">>("Math/Vector3d/Vector3d - Vector3d");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector3s32, SerializedNameSpaceId + "::SubtractVector3s32">>("Math/Vector3s32/Vector3s32 - Vector3s32");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector3s64, SerializedNameSpaceId + "::SubtractVector3s64">>("Math/Vector3s64/Vector3s64 - Vector3s64");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector4f, SerializedNameSpaceId + "::SubtractVector4f32">>("Math/Vector4f/Vector4f - Vector4f");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector4d, SerializedNameSpaceId + "::SubtractVector4f64">>("Math/Vector4d/Vector4d - Vector4d");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector4s32, SerializedNameSpaceId + "::SubtractVector4s32">>("Math/Vector4s32/Vector4s32 - Vector4s32");
        FactoryType::template RegisterNode<SubtractNode<FactoryNodeTypeT, Vector4s64, SerializedNameSpaceId + "::SubtractVector4s64">>("Math/Vector4s64/Vector4s64 - Vector4s64");

        // Multiplication
        FactoryType::template RegisterNode<MultiplyNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::MultiplyF32">>("Math/Float/Float * Float");
        FactoryType::template RegisterNode<MultiplyNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::MultiplyF64">>("Math/Double/Double * Double");
        FactoryType::template RegisterNode<MultiplyNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::MultiplyS32">>("Math/Integer/Integer * Integer");
        FactoryType::template RegisterNode<MultiplyNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::MultiplyS64">>("Math/Integer64/Integer64 * Integer64");

        // Division 
        FactoryType::template RegisterNode<DivisionNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::DivideF32">>("Math/Float/Float \\/ Float");
        FactoryType::template RegisterNode<DivisionNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::DivideF64">>("Math/Double/Double \\/ Double");
        FactoryType::template RegisterNode<DivisionNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::DivideS32">>("Math/Integer/Integer \\/ Integer");
        FactoryType::template RegisterNode<DivisionNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::DivideS64">>("Math/Integer64/Integer64 \\/ Integer64");

        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, onyxF32, SerializedNameSpaceId + "::AbsoluteF32">>("Math/Float/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, onyxF64, SerializedNameSpaceId + "::AbsoluteF64">>("Math/Double/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, onyxS32, SerializedNameSpaceId + "::AbsoluteS32">>("Math/Integer/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, onyxS64, SerializedNameSpaceId + "::AbsoluteS64">>("Math/Integer64/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector2f, SerializedNameSpaceId + "::AbsoluteVector2f32">>("Math/Vector2f/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector2d, SerializedNameSpaceId + "::AbsoluteVector2f64">>("Math/Vector2d/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector2s32, SerializedNameSpaceId + "::AbsoluteVector2s32">>("Math/Vector2s32/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector2s64, SerializedNameSpaceId + "::AbsoluteVector2s64">>("Math/Vector2s64/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector3f, SerializedNameSpaceId + "::AbsoluteVector3f32">>("Math/Vector3f/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector3d, SerializedNameSpaceId + "::AbsoluteVector3f64">>("Math/Vector3d/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector3s32, SerializedNameSpaceId + "::AbsoluteVector3s32">>("Math/Vector3s32/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector3s64, SerializedNameSpaceId + "::AbsoluteVector3s64">>("Math/Vector3s64/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector4f, SerializedNameSpaceId + "::AbsoluteVector4f32">>("Math/Vector4f/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector4d, SerializedNameSpaceId + "::AbsoluteVector4f64">>("Math/Vector4d/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector4s32, SerializedNameSpaceId + "::AbsoluteVector4s32">>("Math/Vector4s32/Absolute");
        FactoryType::template RegisterNode<AbsoluteNode<FactoryNodeTypeT, Vector4s64, SerializedNameSpaceId + "::AbsoluteVector4s64">>("Math/Vector4s64/Absolute");
    }
}
