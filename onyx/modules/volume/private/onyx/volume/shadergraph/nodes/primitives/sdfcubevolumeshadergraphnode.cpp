#include <onyx/volume/shadergraph/nodes/primitives/sdfcubevolumeshadergraphnode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::volume
{
    void SdfCubeVolumeShaderGraphNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {
        
    }

    void SdfCubeVolumeShaderGraphNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin0>() == false) && (context.IsPinConnected<OutPin1>() == false))
            return;

        const InPin0& inputPin0 = GetInputPin0();
        const InPin1& inputPin1 = GetInputPin1();

        generator.AddInclude(rhi::ShaderStage::All, "includes/volume/csg/cube.h");

        String cubeVariableName = format::Format("cubeNode_{:x}", GetId().Get());
        String sampleVariableName = format::Format("cubeSample_{:x}", GetId().Get());
        String isoValueOutVariableName = format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String gradientOutVariableName = format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(format::Format("CsgCube {} = CsgCube({}, {});\n", cubeVariableName,
            inputPin0.IsConnected() ? format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()),
            inputPin1.IsConnected() ? format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>())));

        generator.AppendCode(format::Format("vec4 {} = GetValueAndGradient(worldPosition, {});\n", sampleVariableName, cubeVariableName));
        generator.AppendCode(format::Format("float {} = {}.w;\n", isoValueOutVariableName, sampleVariableName));
        generator.AppendCode(format::Format("vec3 {} = {}.xyz;\n", gradientOutVariableName, sampleVariableName));
    }

#if ONYX_IS_EDITOR
    StringView SdfCubeVolumeShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin0::LocalId: return "Position";
            case InPin1::LocalId: return "Half Extents";
            case OutPin0::LocalId: return "Iso Value";
            case OutPin1::LocalId: return "Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }

#endif

}
