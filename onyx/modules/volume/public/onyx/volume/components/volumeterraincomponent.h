#pragma once
#include <onyx/assets/asset.h>

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

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawImGuiEditor();
#endif

        // the dimension limits of the terrain in each axis
        // if e.g. x is set to 10 and chunk size is 32, the grid ranges max from -16.000 to 16.000
        // if e.g. x is set to -1 it can infinitely scale on the x axis
        Vector3s16 Dimensions { -1, -1, -1 };

        onyxU32 ChunkSize = 64; // world size of the smallest chunk
        onyxU32 Resolution = 512; // resolution of a chunk (voxel size equals ChunkSize / Resolution)

        onyxF32 MaxGeometricError = 0.1f;
        onyxF32 ComplexSurfaceThreshold = 0.89f;

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
        onyxU8 Depth = 0;
    };

    struct VolumeGenerationComponent
    {
        Graphics::ShaderInstanceHandle ResetBuffersShader;
        Graphics::ShaderInstanceHandle UpdateWorldOctreeShader;
        Graphics::ShaderInstanceHandle UpdateWorldOctreeChunkShader;
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
