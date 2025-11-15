#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Volume
{
    void VolumeShaderGraphOutNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {
    }

    void VolumeShaderGraphOutNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != Graphics::ShaderStage::Fragment)
            return;

        const InPin0& inputPin0 = GetInputPin0();
        const InPin1& inputPin1 = GetInputPin1();

        generator.AppendCode(Format::Format("return vec4({}, {});\n",
            inputPin1.IsConnected() ? Format::Format("pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin0>()),
            inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin1>())));
    }

#if ONYX_IS_EDITOR
    StringView VolumeShaderGraphOutNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin0::LocalId: return "Iso Value";
            case InPin1::LocalId: return "Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }

#endif

}
