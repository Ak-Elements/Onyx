#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Graphics
{
    void FragmentShaderOutNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const
    {
        if (GetInputPin().IsConnected())
        {
            generator.AppendCode(Format::Format("outColor = pin_{:x};", GetInputPin().GetLinkedPinGlobalId().Get()));
        }
        else
        {
            const Vector4f& outColor = context.GetPinData<InPin>();
            generator.AppendCode(Format::Format("outColor = vec4({}, {}, {}, {});", outColor[0], outColor[1], outColor[2], outColor[3]));
        }

    }

#if ONYX_IS_EDITOR
    StringView FragmentShaderOutNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin::LocalId: return "Color";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif

    PBRMaterialShaderOutNode::PBRMaterialShaderOutNode()
    {
        InputPins[0] = MakeUnique<AlbedoInPin>();
        InputPins[1] = MakeUnique<NormalInPin>();
        InputPins[2] = MakeUnique<MetalnessInPin>();
        InputPins[3] = MakeUnique<RoughnessInPin>();
    }

    void PBRMaterialShaderOutNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const
    {
        generator.AppendCode("PBRMaterial material;\n");

        Optional<const AlbedoInPin*> albedoPin = GetInputPinByLocalId<AlbedoInPin>();
        if (albedoPin)
        {
            const AlbedoInPin& albedo = *albedoPin.value();
            generator.AppendCode(Format::Format("material.Albedo = {};\n", 
                albedo.IsConnected() ? Format::Format("pin_{:x}", albedo.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<AlbedoInPin>())));
        }

        Optional<const NormalInPin*> normalPin = GetInputPinByLocalId<NormalInPin>();
        if (normalPin)
        {
            const NormalInPin& normal = *normalPin.value();
            generator.AppendCode(Format::Format("material.Normal = {};\n",
                normal.IsConnected() ? Format::Format("pin_{:x}", normal.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<NormalInPin>())));
        }

        Optional<const MetalnessInPin*> metallicPin = GetInputPinByLocalId<MetalnessInPin>();
        if (metallicPin)
        {
            const MetalnessInPin& metalness = *metallicPin.value();
            generator.AppendCode(Format::Format("material.Metalness = {};\n",
                metalness.IsConnected() ? Format::Format("pin_{:x}", metalness.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<MetalnessInPin>())));
        }

        Optional<const RoughnessInPin*> roughnessPin = GetInputPinByLocalId<RoughnessInPin>();
        if (metallicPin)
        {
            const RoughnessInPin& roughness = *roughnessPin.value();
            generator.AppendCode(Format::Format("material.Roughness = {};\n",
                roughness.IsConnected() ? Format::Format("pin_{:x}", roughness.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<RoughnessInPin>())));
        }
    }
}
