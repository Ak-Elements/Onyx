#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::graphics::shader_graph_nodes
{
    void FragmentShaderOutNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (GetInputPin().IsConnected())
        {
            generator.AppendCode(format::Format("outColor = pin_{:x};", GetInputPin().GetLinkedPinGlobalId().Get()));
        }
        else
        {
            const Vector4f32& outColor = context.GetPinData<InPin>();
            generator.AppendCode(format::Format("outColor = vec4({}, {}, {}, {});", outColor[0], outColor[1], outColor[2], outColor[3]));
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

    void PBRMaterialShaderOutNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        generator.AppendCode("PBRMaterial material;\n");

        Optional<const AlbedoInPin*> albedoPin = GetInputPinByLocalId<AlbedoInPin>();
        if (albedoPin)
        {
            const AlbedoInPin& albedo = *albedoPin.value();
            generator.AppendCode(format::Format("material.Albedo = {};\n", 
                albedo.IsConnected() ? format::Format("pin_{:x}", albedo.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<AlbedoInPin>())));
        }

        Optional<const NormalInPin*> normalPin = GetInputPinByLocalId<NormalInPin>();
        if (normalPin)
        {
            const NormalInPin& normal = *normalPin.value();
            generator.AppendCode(format::Format("material.Normal = {};\n",
                normal.IsConnected() ? format::Format("pin_{:x}", normal.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<NormalInPin>())));
        }

        Optional<const MetalnessInPin*> metallicPin = GetInputPinByLocalId<MetalnessInPin>();
        if (metallicPin)
        {
            const MetalnessInPin& metalness = *metallicPin.value();
            generator.AppendCode(format::Format("material.Metalness = {};\n",
                metalness.IsConnected() ? format::Format("pin_{:x}", metalness.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<MetalnessInPin>())));
        }

        Optional<const RoughnessInPin*> roughnessPin = GetInputPinByLocalId<RoughnessInPin>();
        if (metallicPin)
        {
            const RoughnessInPin& roughness = *roughnessPin.value();
            generator.AppendCode(format::Format("material.Roughness = {};\n",
                roughness.IsConnected() ? format::Format("pin_{:x}", roughness.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<RoughnessInPin>())));
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

    node_graph::PinBase* PBRMaterialShaderOutNode::GetInputPin(onyxU32 index)
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

    const node_graph::PinBase* PBRMaterialShaderOutNode::GetInputPin(onyxU32 index) const
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

    node_graph::PinBase* PBRMaterialShaderOutNode::GetOutputPin(onyxU32 /*index*/)
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const node_graph::PinBase* PBRMaterialShaderOutNode::GetOutputPin(onyxU32 /*index*/) const
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

}
