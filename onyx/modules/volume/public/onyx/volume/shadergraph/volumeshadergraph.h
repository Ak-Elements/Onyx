#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::volume {
class VolumeShaderGraph : public onyx::graphics::ShaderGraph {
    friend struct VolumeShaderGraphSerializer;

  public:
    static constexpr StringId32 TypeId{ "onyx::volume::assets::VolumeShaderGraph" };
    static StringId32 getTypeId() { return TypeId; }

    assets::AssetId getBuildOctreeShader() const { return m_buildOctreeShader; }
    assets::AssetId getFindOctreeNodeShader() const { return m_findOctreeNodeShader; }
    assets::AssetId getGenerateVolumeMeshShader() const { return m_generateVolumeMeshShader; }
    assets::AssetId getRaytraceTerrainShader() const { return m_raytraceTerrainShader; }

  private:
    assets::AssetId m_buildOctreeShader;
    assets::AssetId m_findOctreeNodeShader;
    assets::AssetId m_generateVolumeMeshShader;
    assets::AssetId m_raytraceTerrainShader;
};
} // namespace onyx::volume
