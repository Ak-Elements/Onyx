#include <onyx/graphics/shadergraph/nodes/particlesystemoutnode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    void ParticleSystemOutNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const
    {
        generator.AppendCode(Format::Format("particles_out.Position = {};\n",
            m_PositionInPin.IsConnected() ? Format::Format("pin_{:x}", m_PositionInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<PositionInPin>())));

        generator.AppendCode(Format::Format("particles_out.Velocity = {};\n",
            m_VelocityInPin.IsConnected() ? Format::Format("pin_{:x}", m_VelocityInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<VelocityInPin>())));

        generator.AppendCode(Format::Format("particles_out.Color = {};\n",
            m_ColorInPin.IsConnected() ? Format::Format("pin_{:x}", m_ColorInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<ColorInPin>())));

        generator.AppendCode(Format::Format("particles_out.LifeTime = {};\n",
            m_LifeTimeInPin.IsConnected() ? Format::Format("pin_{:x}", m_LifeTimeInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<LifeTimeInPin>())));
    }

    NodeGraph::PinBase* ParticleSystemOutNode::GetInputPin(onyxU32 index)
    {
        switch (index)
        {
        case 0: return &m_PositionInPin;
        case 1: return &m_VelocityInPin;
        case 2: return &m_ColorInPin;
        case 3: return &m_LifeTimeInPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* ParticleSystemOutNode::GetInputPin(onyxU32 index) const
    {
        switch (index)
        {
        case 0: return &m_PositionInPin;
        case 1: return &m_VelocityInPin;
        case 2: return &m_ColorInPin;
        case 3: return &m_LifeTimeInPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    NodeGraph::PinBase* ParticleSystemOutNode::GetOutputPin(onyxU32 /*index*/)
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* ParticleSystemOutNode::GetOutputPin(onyxU32 /*index*/) const
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

}
