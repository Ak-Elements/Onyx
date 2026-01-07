#pragma once

#include <onyx/graphicscore/shader/generators/shadergenerator.h>

namespace Onyx::Volume
{
    class VolumeShaderGraphGenerator : public Graphics::ShaderGenerator
    {
    public:
        VolumeShaderGraphGenerator();

        String GenerateShader() override;
    };
}