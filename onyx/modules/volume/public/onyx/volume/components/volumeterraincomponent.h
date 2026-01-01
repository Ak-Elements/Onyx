#pragma once
#include <onyx/assets/asset.h>

#include <onyx/entity/componentmeta.h>
#include <onyx/graphics/graphicshandles.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
#include <onyx/volume/terrain/worldsparseoctreenode.h>

namespace Onyx
{

namespace Volume
{
    struct TerrainSettingsComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::TerrainSettingsComponent";
        StringId32 GetTypeId() const { return TypeId; }

        onyxS32 Size =  1 << 14; // world size of the terrain

        onyxU32 ChunkSize = 64; // world size of the smallest chunk
        onyxU32 Resolution = 512; // resolution of a chunk (voxel size equals ChunkSize / Resolution)

        onyxF32 MaxGeometricError = 0.1f;
        onyxF32 ComplexSurfaceThreshold = 0.9f;

        Assets::AssetId VolumeGraphAssetId;
    };

    struct TerrainWorldOctreeComponent
    {
        DynamicArray<Terrain::WorldChunksOctreeNode> Octree;

        Graphics::BufferHandle OctreeGpuBuffer;
        Graphics::BufferHandle OctreeChunksBuffer;

        //TODO: should be an octree / spatial structure in the future
        onyxU32 VolumeObjectsCount = 0;
        Graphics::BufferHandle VolumeObjects;
        Graphics::BufferHandle VolumeObjectsData;

        onyxF32 RootSize = 1.0f;
        onyxU8 MaxDepth = 0;
        onyxU8 ChunkMaxDepth = 0;
    };

    struct VolumeGenerationComponent
    {
        Graphics::ShaderInstanceHandle ResetBuffersShader;
        Graphics::ShaderInstanceHandle UpdateWorldOctreeShader;
        Graphics::ShaderInstanceHandle SetupDispatchGenerateMeshShader;
        Graphics::ShaderInstanceHandle GenerateMeshShader;

        // Terrain Editor shaders
        Graphics::ShaderInstanceHandle RayTraceTerrainShader;
        Graphics::ShaderInstanceHandle FindRayTracedOctreeNodeShader;

        Reference<VolumeShaderGraph> VolumeShaderGraph;

        bool HasLoadedShaders = false;
    };
}
    
template <>
struct Serialization<Volume::TerrainSettingsComponent>
{
    static bool Serialize(Serializer& serializer, const Volume::TerrainSettingsComponent& volumeTerrain);
    static bool Deserialize(const Deserializer& deserializer, Volume::TerrainSettingsComponent& outVolumeTerrain);
};

}
