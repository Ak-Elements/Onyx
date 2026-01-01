#pragma once

#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Graphics
{
    class ParticleSystem : public Assets::Asset<ParticleSystem>
    {
        friend struct ParticleSystemSerializer;
    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::ParticleSystem" };
        StringId32 GetTypeId() const { return TypeId; }

        bool IsValid() const { return m_EmitShader.IsValid() && m_UpdateShader.IsValid(); }

    private:
        BufferHandle m_Particles;
        InplaceArray<BufferHandle, 2> m_AliveParticles;
        BufferHandle m_DeadParticles;
        BufferHandle m_ParticleCounts;

        Reference<ShaderGraph> m_EmitShader;
        Reference<ShaderGraph> m_UpdateShader;
    };
}
