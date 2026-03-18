#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::volume
{
    class VolumeShaderGraph : public onyx::graphics::ShaderGraph
    {
        friend struct VolumeShaderGraphSerializer;

    public:
        static constexpr StringId32 TypeId{ "onyx::volume::assets::VolumeShaderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        assets::AssetId GetBuildOctreeShader() const { return m_BuildOctreeShader; }
        assets::AssetId GetFindOctreeNodeShader() const { return m_FindOctreeNodeShader; }
        assets::AssetId GetGenerateVolumeMeshShader() const { return m_GenerateVolumeMeshShader; }
        assets::AssetId GetRaytraceTerrainShader() const { return m_RaytraceTerrainShader; }

    private:
        assets::AssetId m_BuildOctreeShader;
        assets::AssetId m_FindOctreeNodeShader;
        assets::AssetId m_GenerateVolumeMeshShader;
        assets::AssetId m_RaytraceTerrainShader;
    };
}
