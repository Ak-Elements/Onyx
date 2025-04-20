#pragma once
#include <onyx/nodegraph/nodes/math/vectornodes.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::NodeGraph
{
    template <typename InVectorT, typename OutVectorT>
    class SwizzleVectorComponentsNode<Graphics::ShaderGraphNode, InVectorT, OutVectorT> : public FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, InVectorT, OutVectorT>
    {
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

        Vector4u8 OutIndexMapping { 0, 1, 2, 3 };
    };

    template <typename ScalarT>
    class GetVector2Components<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector2<ScalarT>, ScalarT, ScalarT>
    {
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

    template <typename ScalarT>
    class GetVector3Components<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_1_In_3_Out<Graphics::ShaderGraphNode, Vector3<ScalarT>, ScalarT, ScalarT, ScalarT>
    {
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

    template <typename ScalarT>
    class GetVector4Components<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_1_In_4_Out<Graphics::ShaderGraphNode, Vector4<ScalarT>, ScalarT, ScalarT, ScalarT, ScalarT>
    {
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

    template <typename ScalarT>
    class CreateVector2<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_2_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, Vector2<ScalarT>>
    {
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


    template <typename ScalarT>
    class CreateVector3<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_3_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, Vector3<ScalarT>>
    {
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

    template <typename ScalarT>
    class CreateVector4<Graphics::ShaderGraphNode, ScalarT> : public FixedPinNode_4_In_1_Out<Graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, ScalarT, Vector4<ScalarT>>
    {
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
}
