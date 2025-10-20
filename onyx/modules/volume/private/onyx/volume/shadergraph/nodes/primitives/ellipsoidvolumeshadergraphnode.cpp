#include <onyx/volume/shadergraph/nodes/primitives/ellipsoidvolumeshadergraphnode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Volume
{
    EllipsoidVolumeShaderGraphNode::EllipsoidVolumeShaderGraphNode()
    {
        m_DebugName = "CsgSphere";
    }

    void EllipsoidVolumeShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {
        
    }

    void EllipsoidVolumeShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != Graphics::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin0>() == false) && (context.IsPinConnected<OutPin1>() == false))
            return;

        const InPin0& inputPin0 = GetInputPin0();
        const InPin1& inputPin1 = GetInputPin1();

        generator.AddInclude(Graphics::ShaderStage::All, "includes/volume/csg/ellipsoid.h");

        String ellipsoidVariableName = Format::Format("ellipsoidNode_{:x}", GetId().Get());
        String sampleVariableName = Format::Format("ellipsoidSample_{:x}", GetId().Get());
        String outPin0VariableName = Format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String outPin1VariableName = Format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(Format::Format("CsgEllipsoid {} = CsgEllipsoid({}, {});\n", ellipsoidVariableName,
            inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()),
            inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>())));

        generator.AppendCode(Format::Format("vec4 {} = GetValueAndGradient(worldPosition, {});\n", sampleVariableName, ellipsoidVariableName));
        generator.AppendCode(Format::Format("vec3 {} = {}.xyz;\n", outPin0VariableName, sampleVariableName));
        generator.AppendCode(Format::Format("float {} = {}.w;\n", outPin1VariableName, sampleVariableName));
    }

#if ONYX_IS_EDITOR
    StringView EllipsoidVolumeShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin0::LocalId: return "Position";
            case InPin1::LocalId: return "Radii";
            case OutPin0::LocalId: return "Gradient";
            case OutPin1::LocalId: return "Iso Value";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }

#endif

}
