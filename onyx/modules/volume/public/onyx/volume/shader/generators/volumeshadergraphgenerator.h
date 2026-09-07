#pragma once

#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
class VolumeShaderGraphGenerator : public rhi::ShaderGenerator {
  public:
    VolumeShaderGraphGenerator();

  private:
    void generateShaderStage( rhi::ShaderGenerator::StageGenerationContext& stageContext ) override;
};
} // namespace onyx::volume
