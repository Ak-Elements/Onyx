#pragma once

#include <onyx/nodegraph/node.h>
#include <onyx/geometry/vector.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/nodegraph/nodes/math/vectornodes_editor.h>

namespace Onyx::NodeGraph
{
    template <typename NodeType, typename InVectorT, typename OutVectorT>
    class SwizzleVectorComponentsNode : public FixedPinNode_1_In_1_Out<NodeType, InVectorT, OutVectorT>
    {
    private:
        using Super = FixedPinNode_1_In_1_Out<NodeType, InVectorT, OutVectorT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            InVectorT inValue = context.GetPinData<typename Super::InPin>();
            OutVectorT& outValue = context.GetPinData<typename Super::OutPin>();

            if constexpr (is_specialization_of_v<Vector2, OutVectorT>)
            {
                outValue = OutVectorT(OutIndexMapping[0], OutIndexMapping[1]);
            }
            else if constexpr (is_specialization_of_v<Vector3, OutVectorT>)
            {
                outValue = OutVectorT(OutIndexMapping[0], OutIndexMapping[1], OutIndexMapping[2]);
            }
            else if constexpr (is_specialization_of_v<Vector4, OutVectorT>)
            {
                outValue = OutVectorT(OutIndexMapping[0], OutIndexMapping[1], OutIndexMapping[2], OutIndexMapping[3]);
            }
        }

    protected:
        bool OnSerialize(FileSystem::JsonValue& json) const override
        {
            //Super::OnSerialize(json);
            json.Set("swizzleMask", Mask);
            json.Set("swizzle", OutIndexMapping);
            return true;
        }

        bool OnDeserialize(const FileSystem::JsonValue& json) override
        {
            json.Get("swizzleMask", Mask);
            json.Get("swizzle", OutIndexMapping);
            return true;
        }

#if ONYX_IS_EDITOR
    protected:
        bool OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData) override
        {
            bool hasModified = Super::OnDrawInPropertyGrid(constantPinData);

            Optional<Vector4u8> swizzle = Editor::EditorSwizzleVectorControl<OutVectorT, InVectorT>::Draw(Mask);
            if (swizzle.has_value())
            {
                hasModified = true;
                OutIndexMapping = swizzle.value();
            }

            return hasModified;
        }
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin::LocalId: return "In";
                case Super::OutPin::LocalId: return Enums::ToString(Mask);
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif

    private:
        SwizzleMask Mask = is_specialization_of_v<Vector2, OutVectorT> ?
            SwizzleMask::XY :
            is_specialization_of_v<Vector3, OutVectorT> ?
            SwizzleMask::XYZ :
            SwizzleMask::XYW;

        Vector4u8 OutIndexMapping{ 0, 1, 2, 3 };
    };

    template <typename NodeType, typename ScalarT>
    class GetVector2Components : public FixedPinNode_1_In_2_Out<NodeType, Vector2<ScalarT>, ScalarT, ScalarT>
    {
    private:
        using Super = FixedPinNode_1_In_2_Out<NodeType, Vector2<ScalarT>, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            Vector2<ScalarT> inPin = context.GetPinData<typename Super::InPin>();
            
            ScalarT& out0 = context.GetPinData<typename Super::OutPin0>();
            out0 = inPin[0];

            ScalarT& out1 = context.GetPinData<typename Super::OutPin1>();
            out1 = inPin[1];
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin::LocalId: return "In";
                case Super::OutPin0::LocalId: return "X";
                case Super::OutPin1::LocalId: return "Y";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename NodeType, typename ScalarT>
    class GetVector3Components : public FixedPinNode_1_In_3_Out<NodeType, Vector3<ScalarT>, ScalarT, ScalarT, ScalarT>
    {
    private:
        using Super = FixedPinNode_1_In_3_Out<NodeType, Vector3<ScalarT>, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            Vector3<ScalarT> inPin = context.GetPinData<typename Super::InPin>();

            ScalarT& out0 = context.GetPinData<typename Super::OutPin0>();
            out0 = inPin[0];

            ScalarT& out1 = context.GetPinData<typename Super::OutPin1>();
            out1 = inPin[1];

            ScalarT& out2 = context.GetPinData<typename Super::OutPin2>();
            out2 = inPin[2];
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin::LocalId: return "In";
                case Super::OutPin0::LocalId: return "X";
                case Super::OutPin1::LocalId: return "Y";
                case Super::OutPin2::LocalId: return "Z";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename NodeType, typename ScalarT>
    class GetVector4Components : public FixedPinNode_1_In_4_Out<NodeType, Vector4<ScalarT>, ScalarT, ScalarT, ScalarT, ScalarT>
    {
    private:
        using Super = FixedPinNode_1_In_4_Out<NodeType, Vector4<ScalarT>, ScalarT, ScalarT, ScalarT, ScalarT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            Vector4<ScalarT> inPin = context.GetPinData<typename Super::InPin>();

            ScalarT& out0 = context.GetPinData<typename Super::OutPin0>();
            out0 = inPin[0];

            ScalarT& out1 = context.GetPinData<typename Super::OutPin1>();
            out1 = inPin[1];

            ScalarT& out2 = context.GetPinData<typename Super::OutPin2>();
            out2 = inPin[2];

            ScalarT& out3 = context.GetPinData<typename Super::OutPin2>();
            out3 = inPin[3];
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin::LocalId: return "In";
                case Super::OutPin0::LocalId: return "X";
                case Super::OutPin1::LocalId: return "Y";
                case Super::OutPin2::LocalId: return "Z";
                case Super::OutPin3::LocalId: return "W";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename NodeType, typename ScalarT>
    class CreateVector2 : public FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, Vector2<ScalarT>>
    {
    private:
        using Super = FixedPinNode_2_In_1_Out<NodeType, ScalarT, ScalarT, Vector2<ScalarT>>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT inPin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT inPin1 = context.GetPinData<typename Super::InPin1>();

            Vector2<ScalarT>& out = context.GetPinData<typename Super::OutPin>();
            out = Vector2(inPin0, inPin1);
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin0::LocalId: return "X";
                case Super::InPin1::LocalId: return "Y";
                case Super::OutPin::LocalId: return "Vector2";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };


    template <typename NodeType, typename ScalarT>
    class CreateVector3 : public FixedPinNode_3_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT, Vector3<ScalarT>>
    {
    private:
        using Super = FixedPinNode_3_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT, Vector3<ScalarT>>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT inPin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT inPin1 = context.GetPinData<typename Super::InPin1>();
            ScalarT inPin2 = context.GetPinData<typename Super::InPin2>();

            Vector3<ScalarT>& out = context.GetPinData<typename Super::OutPin>();
            out = Vector3(inPin0, inPin1, inPin2);
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin0::LocalId: return "X";
                case Super::InPin1::LocalId: return "Y";
                case Super::InPin2::LocalId: return "Z";
                case Super::OutPin::LocalId: return "Vector3";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename NodeType, typename ScalarT>
    class CreateVector4: public FixedPinNode_4_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT, ScalarT, Vector4<ScalarT>>
    {
    private:
        using Super = FixedPinNode_4_In_1_Out<NodeType, ScalarT, ScalarT, ScalarT, ScalarT, Vector4<ScalarT>>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT inPin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT inPin1 = context.GetPinData<typename Super::InPin1>();
            ScalarT inPin2 = context.GetPinData<typename Super::InPin2>();
            ScalarT inPin3 = context.GetPinData<typename Super::InPin3>();

            Vector4<ScalarT>& out = context.GetPinData<typename Super::OutPin>();
            out = Vector4(inPin0, inPin1, inPin2, inPin3);
        }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin0::LocalId: return "X";
                case Super::InPin1::LocalId: return "Y";
                case Super::InPin2::LocalId: return "Z";
                case Super::InPin3::LocalId: return "W";
                case Super::OutPin::LocalId: return "Vector4";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };

    template <typename FactoryType>
    void RegisterVectorNodes()
    {
        using FactoryNodeTypeT = typename FactoryType::NodeTypeT;

        // Vector 2
        FactoryType::template RegisterNode<GetVector2Components<FactoryNodeTypeT, onyxF32>>("Math/Vector2f/Get Components");
        FactoryType::template RegisterNode<GetVector2Components<FactoryNodeTypeT, onyxF64>>("Math/Vector2d/Get Components");
        FactoryType::template RegisterNode<GetVector2Components<FactoryNodeTypeT, onyxS32>>("Math/Vector2s32/Get Components");
        FactoryType::template RegisterNode<GetVector2Components<FactoryNodeTypeT, onyxS64>>("Math/Vector2s64/Get Components");

        FactoryType::template RegisterNode<CreateVector2<FactoryNodeTypeT, onyxF32>>("Math/Vector2f/Create Vector2f");
        FactoryType::template RegisterNode<CreateVector2<FactoryNodeTypeT, onyxF64>>("Math/Vector2d/Create Vector2d");
        FactoryType::template RegisterNode<CreateVector2<FactoryNodeTypeT, onyxS32>>("Math/Vector2s32/Create Vector2s32");
        FactoryType::template RegisterNode<CreateVector2<FactoryNodeTypeT, onyxS64>>("Math/Vector2s64/Create Vector2s64");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2f, Vector2f>>("Math/Vector2f/Swizzle Vector2f");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3f, Vector2f>>("Math/Vector2f/Swizzle Vector3f");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4f, Vector2f>>("Math/Vector2f/Swizzle Vector4f");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2d, Vector2d>>("Math/Vector2d/Swizzle Vector2d");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3d, Vector2d>>("Math/Vector2d/Swizzle Vector3d");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4d, Vector2d>>("Math/Vector2d/Swizzle Vector4d");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2s32, Vector2s32>>("Math/Vector2s32/Swizzle Vector2s32");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3s32, Vector2s32>>("Math/Vector2s32/Swizzle Vector3s32");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4s32, Vector2s32>>("Math/Vector2s32/Swizzle Vector4s32");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2s64, Vector2s64>>("Math/Vector2s64/Swizzle Vector2s64");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3s64, Vector2s64>>("Math/Vector2s64/Swizzle Vector3s64");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4s64, Vector2s64>>("Math/Vector2s64/Swizzle Vector4s64");

        // Vector 3
        FactoryType::template RegisterNode<GetVector3Components<FactoryNodeTypeT, onyxF32>>("Math/Vector3f/Get Components");
        FactoryType::template RegisterNode<GetVector3Components<FactoryNodeTypeT, onyxF64>>("Math/Vector3d/Get Components");
        FactoryType::template RegisterNode<GetVector3Components<FactoryNodeTypeT, onyxS32>>("Math/Vector3s32/Get Components");
        FactoryType::template RegisterNode<GetVector3Components<FactoryNodeTypeT, onyxS64>>("Math/Vector3s64/Get Components");

        FactoryType::template RegisterNode<CreateVector3<FactoryNodeTypeT, onyxF32>>("Math/Vector3f/Create Vector3f");
        FactoryType::template RegisterNode<CreateVector3<FactoryNodeTypeT, onyxF64>>("Math/Vector3d/Create Vector3d");
        FactoryType::template RegisterNode<CreateVector3<FactoryNodeTypeT, onyxS32>>("Math/Vector3s32/Create Vector3s32");
        FactoryType::template RegisterNode<CreateVector3<FactoryNodeTypeT, onyxS64>>("Math/Vector3s64/Create Vector3s64");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2f, Vector3f>>("Math/Vector3f/Swizzle Vector2f");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3f, Vector3f>>("Math/Vector3f/Swizzle Vector3f");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4f, Vector3f>>("Math/Vector3f/Swizzle Vector4f");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2d, Vector3d>>("Math/Vector3d/Swizzle Vector2d");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3d, Vector3d>>("Math/Vector3d/Swizzle Vector3d");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4d, Vector3d>>("Math/Vector3d/Swizzle Vector4d");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2s32, Vector3s32>>("Math/Vector3s32/Swizzle Vector2s32");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3s32, Vector3s32>>("Math/Vector3s32/Swizzle Vector3s32");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4s32, Vector3s32>>("Math/Vector3s32/Swizzle Vector4s32");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2s64, Vector3s64>>("Math/Vector3s64/Swizzle Vector2s64");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3s64, Vector3s64>>("Math/Vector3s64/Swizzle Vector3s64");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4s64, Vector3s64>>("Math/Vector3s64/Swizzle Vector4s64");

        // Vector 4
        FactoryType::template RegisterNode<GetVector4Components<FactoryNodeTypeT, onyxF32>>("Math/Vector3f/Get Components");
        FactoryType::template RegisterNode<GetVector4Components<FactoryNodeTypeT, onyxF64>>("Math/Vector3d/Get Components");
        FactoryType::template RegisterNode<GetVector4Components<FactoryNodeTypeT, onyxS32>>("Math/Vector3s32/Get Components");
        FactoryType::template RegisterNode<GetVector4Components<FactoryNodeTypeT, onyxS64>>("Math/Vector3s64/Get Components");

        FactoryType::template RegisterNode<CreateVector4<FactoryNodeTypeT, onyxF32>>("Math/Vector4f/Create Vector4f");
        FactoryType::template RegisterNode<CreateVector4<FactoryNodeTypeT, onyxF64>>("Math/Vector4d/Create Vector4d");
        FactoryType::template RegisterNode<CreateVector4<FactoryNodeTypeT, onyxS32>>("Math/Vector4s32/Create Vector4s32");
        FactoryType::template RegisterNode<CreateVector4<FactoryNodeTypeT, onyxS64>>("Math/Vector4s64/Create Vector4s64");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2f, Vector4f>>("Math/Vector4f/Swizzle Vector2f");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3f, Vector4f>>("Math/Vector4f/Swizzle Vector3f");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4f, Vector4f>>("Math/Vector4f/Swizzle Vector4f");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2d, Vector4d>>("Math/Vector4d/Swizzle Vector2d");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3d, Vector4d>>("Math/Vector4d/Swizzle Vector3d");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4d, Vector4d>>("Math/Vector4d/Swizzle Vector4d");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2s32, Vector4s32>>("Math/Vector4s32/Swizzle Vector2s32");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3s32, Vector4s32>>("Math/Vector4s32/Swizzle Vector3s32");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4s32, Vector4s32>>("Math/Vector4s32/Swizzle Vector4s32");

        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector2s64, Vector4s64>>("Math/Vector4s64/Swizzle Vector2s64");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector3s64, Vector4s64>>("Math/Vector4s64/Swizzle Vector3s64");
        FactoryType::template RegisterNode<SwizzleVectorComponentsNode<FactoryNodeTypeT, Vector4s64, Vector4s64>>("Math/Vector4s64/Swizzle Vector4s64");

    }
}
