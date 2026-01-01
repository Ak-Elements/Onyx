#include <onyx/graphics/shadergraph/nodes/particlesystememitoutnode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    void ParticleSystemEmitOutNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const
    {
        generator.AppendCode(Format::Format("particles_out.Position = {};\n",
            m_SpawnPositionInPin.IsConnected() ? Format::Format("pin_{:x}", m_SpawnPositionInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<SpawnPositionInPin>())));

        generator.AppendCode(Format::Format("particles_out.Velocity = {};\n",
            m_SpawnVelocityInPin.IsConnected() ? Format::Format("pin_{:x}", m_SpawnVelocityInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<SpawnVelocityInPin>())));

        generator.AppendCode(Format::Format("particles_out.Color = {};\n",
            m_SpawnColorInPin.IsConnected() ? Format::Format("pin_{:x}", m_SpawnColorInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<SpawnColorInPin>())));

        generator.AppendCode(Format::Format("particles_out.LifeTime = {};\n",
            m_SpawnLifeTimeInPin.IsConnected() ? Format::Format("pin_{:x}", m_SpawnLifeTimeInPin.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<LifeTimeInPin>())));
    }

    NodeGraph::PinBase* ParticleSystemEmitOutNode::GetInputPin(onyxU32 index)
    {
        switch (index)
        {
        case 0: return &m_SpawnVelocityInPin;
        case 1: return &m_SpawnVelocityInPin;
        case 2: return &m_SpawnColorInPin;
        case 3: return &m_SpawnLifeTimeInPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* ParticleSystemEmitOutNode::GetInputPin(onyxU32 index) const
    {
        switch (index)
        {
        case 0: return &m_SpawnVelocityInPin;
        case 1: return &m_SpawnVelocityInPin;
        case 2: return &m_SpawnColorInPin;
        case 3: return &m_SpawnLifeTimeInPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    NodeGraph::PinBase* ParticleSystemEmitOutNode::GetOutputPin(onyxU32 /*index*/)
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* ParticleSystemEmitOutNode::GetOutputPin(onyxU32 /*index*/) const
    {
        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

}
