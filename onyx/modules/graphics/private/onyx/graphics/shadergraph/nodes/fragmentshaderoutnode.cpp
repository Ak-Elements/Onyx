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
            const Vector4f32& outColor = context.GetPinData<InPin>();
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

#if ONYX_IS_EDITOR
    std::any PBRMaterialShaderOutNode::CreateDefaultForPin(StringId32 pinId) const
    {
        if (pinId == RoughnessInPin::LocalId)
        {
            return 0.5f;
        }

        return Super::CreateDefaultForPin(pinId);
    }
#endif

    NodeGraph::PinBase* PBRMaterialShaderOutNode::GetInputPin(onyxU32 index)
    {
        switch (index)
        {
        case 0: return &m_AlbedoInPin;
        case 1: return &m_NormalInPin;
        case 2: return &m_MetalnessInPin;
        case 3: return &m_RoughnessInPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* PBRMaterialShaderOutNode::GetInputPin(onyxU32 index) const
    {
        switch (index)
        {
        case 0: return &m_AlbedoInPin;
        case 1: return &m_NormalInPin;
        case 2: return &m_MetalnessInPin;
        case 3: return &m_RoughnessInPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    NodeGraph::PinBase* PBRMaterialShaderOutNode::GetOutputPin(onyxU32 /*index*/)
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* PBRMaterialShaderOutNode::GetOutputPin(onyxU32 /*index*/) const
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

}
