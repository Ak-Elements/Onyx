#pragma once
#include <onyx/nodegraph/node.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::NodeGraph
{
    template <typename NodeType, template <typename ScalarT> class VectorT, typename ScalarT, CompileTimeString TypeIdString>
    class ScaleVectorNode : public FixedPinNode_2_In_1_Out<NodeType, VectorT<ScalarT>, ScalarT, VectorT<ScalarT>>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<NodeType, VectorT<ScalarT>, ScalarT, VectorT<ScalarT>>;

        void OnUpdate(ExecutionContext& context) const override
        {
            VectorT<ScalarT> pin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT pin1 = context.GetPinData<typename Super::InPin1>();

            VectorT<ScalarT>& out = context.GetPinData<typename Super::OutPin>();
            out = pin0 * pin1;
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename FactoryType, CompileTimeString SerializedNameSpaceId>
    void RegisterGeometricNodes()
    {
        using FactoryNodeTypeT = typename FactoryType::NodeTypeT;

        // Scale
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector2, onyxF32, SerializedNameSpaceId + "::ScaleVector2f32">>("Geometric/Vector2f/Scale Vector2f");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector2, onyxF64, SerializedNameSpaceId + "::ScaleVector2f64">>("Geometric/Vector2d/Scale Vector2d");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector2, onyxS32, SerializedNameSpaceId + "::ScaleVector2s32">>("Geometric/Vector2s32/Scale Vector2s32");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector2, onyxS64, SerializedNameSpaceId + "::ScaleVector2s64">>("Geometric/Vector2s64/Scale Vector2s64");

        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector3, onyxF32, SerializedNameSpaceId + "::ScaleVector3f32">>("Geometric/Vector3f/Scale Vector3f");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector3, onyxF64, SerializedNameSpaceId + "::ScaleVector3f64">>("Geometric/Vector3d/Scale Vector3d");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector3, onyxS32, SerializedNameSpaceId + "::ScaleVector3s32">>("Geometric/Vector3s32/Scale Vector3s32");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector3, onyxS64, SerializedNameSpaceId + "::ScaleVector3s64">>("Geometric/Vector3s64/Scale Vector3s64");

        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector4, onyxF32, SerializedNameSpaceId + "::ScaleVector4f32">>("Geometric/Vector4f/Scale Vector4f");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector4, onyxF64, SerializedNameSpaceId + "::ScaleVector4f64">>("Geometric/Vector4d/Scale Vector4d");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector4, onyxS32, SerializedNameSpaceId + "::ScaleVector4s32">>("Geometric/Vector4s32/Scale Vector4s32");
        FactoryType::template RegisterNode<ScaleVectorNode<FactoryNodeTypeT, Vector4, onyxS64, SerializedNameSpaceId + "::ScaleVector4s64">>("Geometric/Vector4s64/Scale Vector4s64");
    }
}
