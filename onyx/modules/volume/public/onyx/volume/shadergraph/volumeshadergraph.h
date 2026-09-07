#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::volume {
class VolumeShaderGraph : public onyx::graphics::ShaderGraph {
    friend struct VolumeShaderGraphSerializer;

  public:
    static constexpr StringId32 TypeId{ "onyx::volume::assets::VolumeShaderGraph" };
    static StringId32 getTypeId() { return TypeId; }

    assets::AssetId getRenderTerrainShader() const { return m_renderTerrainShader; }

  private:
    assets::AssetId m_renderTerrainShader;
};
} // namespace onyx::volume
