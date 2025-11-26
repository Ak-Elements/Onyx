#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Volume
{
    class VolumeShaderGraph : public Onyx::Graphics::ShaderGraph
    {
        friend struct VolumeShaderGraphSerializer;

    public:
        static constexpr StringId32 TypeId{ "Onyx::Volume::Assets::VolumeShaderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        Assets::AssetId GetBuildOctreeShader() const { return m_BuildOctreeShader; }
        Assets::AssetId GetFindOctreeNodeShader() const { return m_FindOctreeNodeShader; }
        Assets::AssetId GetGenerateVolumeMeshShader() const { return m_GenerateVolumeMeshShader; }
        Assets::AssetId GetRaytraceTerrainShader() const { return m_RaytraceTerrainShader; }

    private:
        Assets::AssetId m_BuildOctreeShader;
        Assets::AssetId m_FindOctreeNodeShader;
        Assets::AssetId m_GenerateVolumeMeshShader;
        Assets::AssetId m_RaytraceTerrainShader;
    };
}
