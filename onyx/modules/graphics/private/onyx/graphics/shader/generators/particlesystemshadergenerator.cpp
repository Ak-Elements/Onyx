#include <onyx/graphics/shader/generators/particlesystemshadergenerator.h>

namespace Onyx::Graphics
{
    ParticleSystemShaderGenerator::ParticleSystemShaderGenerator()
    {
        m_CurrentStage = ShaderStage::Compute;

        AddPushConstant("DeltaTime", ShaderDataType::Float);
    }

    String ParticleSystemShaderGenerator::GenerateShaderEmit()
    {
        String includes;
        for (const String& include : m_ShaderStagesIncludes[Enums::ToIntegral(Graphics::ShaderStage::All)])
        {
            includes += Format::Format("#include \"{}\"\n", include);
        }

        for (const String& include : m_ShaderStagesIncludes[Enums::ToIntegral(m_CurrentStage)])
        {
            includes += Format::Format("#include \"{}\"\n", include);
        }

        String particleCode = m_ShaderStagesCode[Enums::ToIntegral(m_CurrentStage)];
        // TODO: currently all node generated code is added as fragment code
        return Format::Format(R"(
                #version 460 core
                {}
                
                compute
                {{
                    layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
                    
                    // TODO: This should be data driven based on the emit / update shader graph
                    struct Particle
                    {
                        vec3 Position;
                        float LifeTime;
                        vec3 Velocity;
                        vec3 Color;
                    };

                    layout(std430, buffer_reference, buffer_reference_align = 8) buffer ParticlesBuffer
                    {
                        Particle Particles[];
                    };

                    layout(std430, buffer_reference, buffer_reference_align = 4) buffer ParticlesControlBlockBuffer
                    {
                        uint AliveParticleCount;
                        uint DeadParticleCount;
                        uint MaxCount;
                    };

                    layout(std430, buffer_reference, buffer_reference_align = 4) buffer ParticleIndicesBuffer
                    {
                        uint Indices[];
                    };
                    
                    layout(push_constant) uniform Constants
                    {
                        ParticlesBuffer ParticlesPtr[10000];
                        ParticlesControlBlockBuffer ParticlesControlPtr;

                        ParticleIndicesBuffer AliveIndicesPtr;
                        ParticleIndicesBuffer DeadIndicesPtr;

                        float DeltaTime;
                    } constants;

                    
                    void main()
                    {{
                        check for dead particles// - update particle lifetime
                        
                        if (gl_GlobalInvocationID.x > spawnCount)
                            return;
                        
                        if (constants.ParticlesControlPtr.DeadParticleCount == 0)
                            return;

                        if (constants.ParticlesControlPtr.AliveParticleCount == constants.ParticlesControlPtr.MaxCount)
                            return;

                        uint nextDeadParticle = atomicSub(constants.ParticlesControlPtr.DeadParticleCount, 1);
                        uint particleIndex = constants.DeadIndicesPtr[nextDeadParticle];

                        uint newActiveCount = atomicAdd(constants.ParticlesControlPtr.AliveParticleCount, 1);
                        constants.AliveIndicesPtr[newActiveCount] = particleIndex;

                        Particle current;
                        
                        {}

                        constants.ParticlesPtr[particleIndex] = current;
                    }}
                }}
                )",
            includes, 
            particleCode);

    }

    String ParticleSystemShaderGenerator::GenerateShaderUpdate()
    {
        
    }

    String ParticleSystemShaderGenerator::GenerateShader()
    {
        return m_ShaderStagesCode[static_cast<onyxU8>(m_CurrentStage)];
    }
}