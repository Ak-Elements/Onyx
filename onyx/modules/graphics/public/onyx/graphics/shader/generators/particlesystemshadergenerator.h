#pragma once

#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Graphics
{
    class ParticleSystemShaderGenerator : public Graphics::ShaderGenerator
    {
    public:
        ParticleSystemShaderGenerator();

        String GenerateShaderEmit();
        String GenerateShaderUpdate();
    };
}