#pragma once
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::NodeGraph
{
    template <typename T, CompileTimeString TypeIdString>
    class RandomNode : public FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, onyxU32, T>
    {
    public:
        static constexpr bool HasAliases = true;

        static constexpr StringId32 TypeId = TypeIdString;
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FixedPinNode_1_In_1_Out<Graphics::ShaderGraphNode, onyxU32, T>;

        void OnUpdate(ExecutionContext& /*context*/) const override
        {
            // Nothing todo
        }

#if ONYX_IS_EDITOR
    private:
        void DoGenerateShader(const ExecutionContext& context, Graphics::ShaderGenerator& generator) const override
        {

            // 
            //float rand(vec2 co)
            //{
            //    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
            //}
            //
            generator.AddInclude(Graphics::ShaderStage::All, "includes/math/random.h");

            ///generator.AppendCode(Format::Format("{} pin_{:x} = ", Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(), Super::GetOutputPin().GetGlobalId().Get()));

            //

            //const typename Super::InPin& seedInputPin = Super::GetInputPin();
            generator.AppendCode(Format::Format("{} pin_{:x} = vec3(random(gl_GlobalInvocationID.xx), random(gl_GlobalInvocationID.xy), random(gl_GlobalInvocationID.yx));",
                Graphics::ShaderGenerator::GetTypeAsShaderTypeString<typename Super::OutPin::DataType>().c_str(),
                Super::GetOutputPin().GetGlobalId().Get(),
                /*seedInputPin.IsConnected() ? seedInputPin.GetLinkedPinGlobalId().Get() :*/ Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>())));
        }
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin::LocalId: return "Seed";
            case Super::OutPin::LocalId: return "Random";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}

namespace Onyx::Graphics::ShaderGraphNodes
{
    using RandomNodeVector2f32 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector2f32, "Onyx::Graphics::ShaderGraph::RandomVector2f32" >;
    using RandomNodeVector2f64 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector2f64, "Onyx::Graphics::ShaderGraph::RandomVector2f64" >;
    using RandomNodeVector2s32 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector2s32, "Onyx::Graphics::ShaderGraph::RandomVector2s32" >;
    using RandomNodeVector2s64 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector2s64, "Onyx::Graphics::ShaderGraph::RandomVector2s64">;
                                                      
    using RandomNodeVector3f32 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector3f32, "Onyx::Graphics::ShaderGraph::RandomVector3f32">;
    using RandomNodeVector3f64 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector3f64, "Onyx::Graphics::ShaderGraph::RandomVector3f64">;
    using RandomNodeVector3s32 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector3s32, "Onyx::Graphics::ShaderGraph::RandomVector3s32">;
    using RandomNodeVector3s64 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector3s64, "Onyx::Graphics::ShaderGraph::RandomVector3s64">;
                                                      
    using RandomNodeVector4f32 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector4f32, "Onyx::Graphics::ShaderGraph::RandomVector4f32">;
    using RandomNodeVector4f64 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector4f64, "Onyx::Graphics::ShaderGraph::RandomVector4f64">;
    using RandomNodeVector4s32 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector4s32, "Onyx::Graphics::ShaderGraph::RandomVector4s32">;
    using RandomNodeVector4s64 = NodeGraph::RandomNode</*ShaderGraphNode,*/ Vector4s64, "Onyx::Graphics::ShaderGraph::RandomVector4s64">;
}
