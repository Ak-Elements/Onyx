#pragma once
#include <onyx/nodegraph/nodes/math/vectornodes.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::NodeGraph
{
    template <typename InVectorT, typename OutVectorT, CompileTimeString TypeIdString>
    class SwizzleVectorComponentsNode<Graphics::ShaderGraphNode, InVectorT, OutVectorT, TypeIdString> : public FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, InVectorT, OutVectorT>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, InVectorT, OutVectorT>;

        void OnUpdate(ExecutionContext& context) const override
        {
            InVectorT inValue = context.GetPinData<typename Super::InPin>();
            OutVectorT& outValue = context.GetPinData<typename Super::OutPin>();

            if constexpr (is_specialization_of_v<Vector2, OutVectorT>)
            {
                outValue = OutVectorT(inValue[OutIndexMapping[0]], inValue[OutIndexMapping[1]]);
            }
            else if constexpr (is_specialization_of_v<Vector3, OutVectorT>)
            {
                outValue = OutVectorT(inValue[OutIndexMapping[0]], inValue[OutIndexMapping[1]], inValue[OutIndexMapping[2]]);
            }
            else if constexpr (is_specialization_of_v<Vector4, OutVectorT>)
            {
                outValue = OutVectorT(inValue[OutIndexMapping[0]], inValue[OutIndexMapping[1]], inValue[OutIndexMapping[2]], inValue[OutIndexMapping[3]]);
            }
        }

    protected:
        bool OnSerialize(Serializer& serializer) const override
        {
            //Super::OnSerialize(json);
            serializer.Write<"swizzleMask">(Mask);
            serializer.Write<"swizzle">(OutIndexMapping);
            return true;
        }

        bool OnDeserialize(const Deserializer& deserializer) override
        {
            deserializer.Read<"swizzleMask">(Mask);
            deserializer.Read<"swizzle">(OutIndexMapping);
            return true;
        }

#if ONYX_IS_EDITOR
        bool OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData) override
        {
            bool hasModified = Super::OnDrawInPropertyGrid(constantPinData);

            //Optional<Vector4u8> swizzle = Editor::EditorSwizzleVectorControl<InVectorT, OutVectorT>::Draw(Mask);
            //if (swizzle.has_value())
            //{
            //    hasModified = true;
            //    OutIndexMapping = swizzle.value();
            //}

            return hasModified;
        }

        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin& inputPin = Super::GetInputPin();

            generator.AppendCode(Format::Format("{} pin_{:x} = {}.{}; \n",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<OutVectorT>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                inputPin.IsConnected() ? Format::Format("pin_{:x}", inputPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>()),
                ToLower(Enums::ToString(Mask))));
        }

    private:
        StringView GetPinName(StringId32 pinId) const override
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

        Vector4u8 OutIndexMapping { 0, 1, 2, 3 };
    };

    template <typename ScalarT, CompileTimeString TypeIdString>
    class GetVector2Components<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector2<ScalarT>, ScalarT, ScalarT>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector2<ScalarT>, ScalarT, ScalarT>;

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
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin& inputPin = Super::GetInputPin();
            if (inputPin.IsConnected())
            {
                if (context.IsPinConnected<typename Super::OutPin0>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.x; // X \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin0().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }

                if (context.IsPinConnected<typename Super::OutPin1>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.y; // Y \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin1().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }
            }
            else
            {
                Vector2<ScalarT> inPin = context.GetPinData<typename Super::InPin>();

                if (context.IsPinConnected<typename Super::OutPin0>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // X \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin0().GetGlobalId().Get(),
                        inPin[0]));
                }

                if (context.IsPinConnected<typename Super::OutPin1>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // Y \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin1().GetGlobalId().Get(),
                        inPin[1]));
                }
            }
        }

    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename ScalarT, CompileTimeString TypeIdString>
    class GetVector3Components<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_1_In_3_Out<Graphics::ShaderGraphNode, Vector3<ScalarT>, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_1_In_3_Out<Graphics::ShaderGraphNode, Vector3<ScalarT>, ScalarT, ScalarT, ScalarT>;

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
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin& inputPin = Super::GetInputPin();

            if (inputPin.IsConnected())
            {
                if (context.IsPinConnected<typename Super::OutPin0>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.x; // X \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin0().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }

                if (context.IsPinConnected<typename Super::OutPin1>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.y; // Y \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin1().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }

                if (context.IsPinConnected<typename Super::OutPin2>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.z; // Z \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin2().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }
            }
            else
            {
                Vector3<ScalarT> inPin = context.GetPinData<typename Super::InPin>();

                if (context.IsPinConnected<typename Super::OutPin0>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // X \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin0().GetGlobalId().Get(),
                        inPin[0]));
                }

                if (context.IsPinConnected<typename Super::OutPin1>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // Y \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin1().GetGlobalId().Get(),
                        inPin[1]));
                }

                if (context.IsPinConnected<typename Super::OutPin2>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // Z \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin2().GetGlobalId().Get(),
                        inPin[2]));
                }
            }
        }

    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename ScalarT, CompileTimeString TypeIdString>
    class GetVector4Components<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_1_In_4_Out<Graphics::ShaderGraphNode, Vector4<ScalarT>, ScalarT, ScalarT, ScalarT, ScalarT>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_1_In_4_Out<Graphics::ShaderGraphNode, Vector4<ScalarT>, ScalarT, ScalarT, ScalarT, ScalarT>;

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
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin& inputPin = Super::GetInputPin();

            if (inputPin.IsConnected())
            {
                if (context.IsPinConnected<typename Super::OutPin0>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.x; // X \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin0().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }

                if (context.IsPinConnected<typename Super::OutPin1>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.y; // Y \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin1().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }

                if (context.IsPinConnected<typename Super::OutPin2>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.z; // Z \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin2().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }

                if (context.IsPinConnected<typename Super::OutPin3>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = pin_{:x}.w; // W \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin3().GetGlobalId().Get(),
                        inputPin.GetLinkedPinGlobalId().Get()));
                }
            }
            else
            {
                Vector4<ScalarT> inPin = context.GetPinData<typename Super::InPin>();

                if (context.IsPinConnected<typename Super::OutPin0>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // X \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin0().GetGlobalId().Get(),
                        inPin[0]));
                }

                if (context.IsPinConnected<typename Super::OutPin1>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // Y \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin1().GetGlobalId().Get(),
                        inPin[1]));
                }

                if (context.IsPinConnected<typename Super::OutPin2>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // Z \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin2().GetGlobalId().Get(),
                        inPin[2]));
                }

                if (context.IsPinConnected<typename Super::OutPin3>())
                {
                    generator.AppendCode(Format::Format("{} pin_{:x} = {}; // W \n",
                        Graphics::ShaderGenerator::GetTypeAsShaderTypeString<ScalarT>().c_str(),
                        Super::GetOutputPin3().GetGlobalId().Get(),
                        inPin[3]));
                }
            }
        }

    private:
        StringView GetPinName(StringId32 pinId) const override
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

    template <typename ScalarT, CompileTimeString TypeIdString>
    class CreateVector2<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, Vector2<ScalarT>>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, Vector2<ScalarT>>;

        void OnUpdate(ExecutionContext& context) const override
        {
            ScalarT inPin0 = context.GetPinData<typename Super::InPin0>();
            ScalarT inPin1 = context.GetPinData<typename Super::InPin1>();

            Vector2<ScalarT>& out = context.GetPinData<typename Super::OutPin>();
            out = Vector2(inPin0, inPin1);
        }

#if ONYX_IS_EDITOR
    private:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();

            generator.AppendCode(Format::Format("{0} pin_{1:x} = {0}({2}, {3}); \n",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<Vector2<ScalarT>>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()),
                inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>())));
        }

    private:
        StringView GetPinName(StringId32 pinId) const override
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


    template <typename ScalarT, CompileTimeString TypeIdString>
    class CreateVector3<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_3_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, Vector3<ScalarT>>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_3_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, Vector3<ScalarT>>;

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
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
            const typename Super::InPin2& inputPin2 = Super::GetInputPin2();

            generator.AppendCode(Format::Format("{0} pin_{1:x} = {0}({2}, {3}, {4}); \n",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<Vector3<ScalarT>>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()),
                inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>()),
                inputPin2.IsConnected() ? Format::Format("pin_{:x}", inputPin2.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin2>())));
        }

        StringView GetPinName(StringId32 pinId) const override
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

    template <typename ScalarT, CompileTimeString TypeIdString>
    class CreateVector4<Graphics::ShaderGraphNode, ScalarT, TypeIdString> : public FixedPinNode_4_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, ScalarT, Vector4<ScalarT>>
    {
    public:
        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_4_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, ScalarT, Vector4<ScalarT>>;

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
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {
            const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
            const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
            const typename Super::InPin2& inputPin2 = Super::GetInputPin2();
            const typename Super::InPin3& inputPin3 = Super::GetInputPin3();

            generator.AppendCode(Format::Format("{0} pin_{1:x} = {0}({2}, {3}, {4}, {5}); \n",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<Vector4<ScalarT>>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()),
                inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>()),
                inputPin2.IsConnected() ? Format::Format("pin_{:x}", inputPin2.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin2>()),
                inputPin3.IsConnected() ? Format::Format("pin_{:x}", inputPin3.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin3>())));
        }


        StringView GetPinName(StringId32 pinId) const override
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
}

namespace Onyx::Graphics::ShaderGraphNodes
{
    using GetVector2f32Components = NodeGraph::GetVector2Components<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::GetVector2f32Components">;
    using GetVector2f64Components = NodeGraph::GetVector2Components<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::GetVector2f64Components">;
    using GetVector2s32Components = NodeGraph::GetVector2Components<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::GetVector2s32Components">;
    using GetVector2s64Components = NodeGraph::GetVector2Components<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::GetVector2s64Components">;

    using CreateVector2f32 = NodeGraph::CreateVector2<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::CreateVector2f32">;
    using CreateVector2f64 = NodeGraph::CreateVector2<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::CreateVector2f64">;
    using CreateVector2s32 = NodeGraph::CreateVector2<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::CreateVector2s32">;
    using CreateVector2s64 = NodeGraph::CreateVector2<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::CreateVector2s64">;

    using Swizzle2DVector2f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2f32, Vector2f32, "Onyx::Graphics::ShaderGraph::Swizzle2DVector2f32">;
    using Swizzle2DVector3f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3f32, Vector2f32, "Onyx::Graphics::ShaderGraph::Swizzle2DVector3f32">;
    using Swizzle2DVector4f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4f32, Vector2f32, "Onyx::Graphics::ShaderGraph::Swizzle2DVector4f32">;

    using Swizzle2DVector2f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2f64, Vector2f64, "Onyx::Graphics::ShaderGraph::Swizzle2DVector2f64">;
    using Swizzle2DVector3f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3f64, Vector2f64, "Onyx::Graphics::ShaderGraph::Swizzle2DVector3f64">;
    using Swizzle2DVector4f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4f64, Vector2f64, "Onyx::Graphics::ShaderGraph::Swizzle2DVector4f64">;

    using Swizzle2DVector2s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2s32, Vector2s32, "Onyx::Graphics::ShaderGraph::Swizzle2DVector2s32">;
    using Swizzle2DVector3s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3s32, Vector2s32, "Onyx::Graphics::ShaderGraph::Swizzle2DVector3s32">;
    using XSwizzle2DVector4s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4s32, Vector2s32, "Onyx::Graphics::ShaderGraph::Swizzle2DVector4s32">;

    using Swizzle2DVector2s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2s64, Vector2s64, "Onyx::Graphics::ShaderGraph::Swizzle2DVector2s64">;
    using Swizzle2DVector3s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3s64, Vector2s64, "Onyx::Graphics::ShaderGraph::Swizzle2DVector3s64">;
    using Swizzle2DVector4s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4s64, Vector2s64, "Onyx::Graphics::ShaderGraph::Swizzle2DVector4s64">;

    // Vector 3
    using GetVector3f32Components = NodeGraph::GetVector3Components<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::GetVector3f32Components">;
    using GetVector3f64Components = NodeGraph::GetVector3Components<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::GetVector3f64Components">;
    using GetVector3s32Components = NodeGraph::GetVector3Components<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::GetVector3s32Components">;
    using GetVector3s64Components = NodeGraph::GetVector3Components<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::GetVector3s64Components">;

    using CreateVector3f32 = NodeGraph::CreateVector3<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::CreateVector3f32">;
    using CreateVector3f64 = NodeGraph::CreateVector3<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::CreateVector3f64">;
    using CreateVector3s32 = NodeGraph::CreateVector3<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::CreateVector3s32">;
    using CreateVector3s64 = NodeGraph::CreateVector3<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::CreateVector3s64">;

    using Swizzle3DVector2f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2f32, Vector3f32, "Onyx::Graphics::ShaderGraph::Swizzle3DVector2f32">;
    using Swizzle3DVector3f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3f32, Vector3f32, "Onyx::Graphics::ShaderGraph::Swizzle3DVector3f32">;
    using Swizzle3DVector4f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4f32, Vector3f32, "Onyx::Graphics::ShaderGraph::Swizzle3DVector4f32">;

    using Swizzle3DVector2f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2f64, Vector3f64, "Onyx::Graphics::ShaderGraph::Swizzle3DVector2f64">;
    using Swizzle3DVector3f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3f64, Vector3f64, "Onyx::Graphics::ShaderGraph::Swizzle3DVector3f64">;
    using Swizzle3DVector4f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4f64, Vector3f64, "Onyx::Graphics::ShaderGraph::Swizzle3DVector4f64">;

    using Swizzle3DVector2s32  = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2s32, Vector3s32, "Onyx::Graphics::ShaderGraph::Swizzle3DVector2s32">;
    using Swizzle3DVector3s32  = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3s32, Vector3s32, "Onyx::Graphics::ShaderGraph::Swizzle3DVector3s32">;
    using Swizzle3DVector4s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4s32, Vector3s32, "Onyx::Graphics::ShaderGraph::Swizzle3DVector4s32">;

    using Swizzle3DVector2s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2s64, Vector3s64, "Onyx::Graphics::ShaderGraph::Swizzle3DVector2s64">;
    using Swizzle3DVector3s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3s64, Vector3s64, "Onyx::Graphics::ShaderGraph::Swizzle3DVector3s64">;
    using Swizzle3DVector4s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4s64, Vector3s64, "Onyx::Graphics::ShaderGraph::Swizzle3DVector4s64">;

    // Vector 4
    using GetVector4f32Components = NodeGraph::GetVector4Components<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::GetVector4f32Components">;
    using GetVector4f64Components = NodeGraph::GetVector4Components<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::GetVector4f64Components">;
    using GetVector4s32Components = NodeGraph::GetVector4Components<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::GetVector4s32Components">;
    using GetVector4s64Components = NodeGraph::GetVector4Components<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::GetVector4s64Components">;

    using CreateVector4f32 = NodeGraph::CreateVector4<ShaderGraphNode, onyxF32, "Onyx::Graphics::ShaderGraph::CreateVector4f32">;
    using CreateVector4f64 = NodeGraph::CreateVector4<ShaderGraphNode, onyxF64, "Onyx::Graphics::ShaderGraph::CreateVector4f64">;
    using CreateVector4s32 = NodeGraph::CreateVector4<ShaderGraphNode, onyxS32, "Onyx::Graphics::ShaderGraph::CreateVector4s32">;
    using CreateVector4s64 = NodeGraph::CreateVector4<ShaderGraphNode, onyxS64, "Onyx::Graphics::ShaderGraph::CreateVector4s64">;

    using Swizzle4DVector2f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2f32, Vector4f32, "Onyx::Graphics::ShaderGraph::Swizzle4DVector2f32">;
    using Swizzle4DVector3f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3f32, Vector4f32, "Onyx::Graphics::ShaderGraph::Swizzle4DVector3f32">;
    using Swizzle4DVector4f32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4f32, Vector4f32, "Onyx::Graphics::ShaderGraph::Swizzle4DVector4f32">;

    using Swizzle4DVector2f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2f64, Vector4f64, "Onyx::Graphics::ShaderGraph::Swizzle4DVector2f64">;
    using Swizzle4DVector3f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3f64, Vector4f64, "Onyx::Graphics::ShaderGraph::Swizzle4DVector3f64">;
    using Swizzle4DVector4f64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4f64, Vector4f64, "Onyx::Graphics::ShaderGraph::Swizzle4DVector4f64">;

    using Swizzle4DVector2s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2s32, Vector4s32, "Onyx::Graphics::ShaderGraph::Swizzle4DVector2s32">;
    using Swizzle4DVector3s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3s32, Vector4s32, "Onyx::Graphics::ShaderGraph::Swizzle4DVector3s32">;
    using Swizzle4DVector4s32 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4s32, Vector4s32, "Onyx::Graphics::ShaderGraph::Swizzle4DVector4s32">;

    using Swizzle4DVector2s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector2s64, Vector4s64, "Onyx::Graphics::ShaderGraph::Swizzle4DVector2s64">;
    using Swizzle4DVector3s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector3s64, Vector4s64, "Onyx::Graphics::ShaderGraph::Swizzle4DVector3s64">;
    using Swizzle4DVector4s64 = NodeGraph::SwizzleVectorComponentsNode<ShaderGraphNode, Vector4s64, Vector4s64, "Onyx::Graphics::ShaderGraph::Swizzle4DVector4s64">;
}
