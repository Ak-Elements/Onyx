#pragma once
#include <onyx/assets/asset.h>

#include <onyx/graphics/graphicshandles.h>
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

        // if the chunksize is 32x32 and the resolution is 32x32, each voxel is 1x1.
        // if the resolution is set to 16x16 each voxel is 2x2
        // resolution 64x64 - each voxel is 0.5x0.5x
        onyxU32 ChunkSize = 32; // size per volume chunk
        onyxU32 Resolution = 32; // resolution per chunk

        onyxU32 MinimumVoxelSizeInCentimeters = 50;
    };

    struct TerrainWorldOctreeComponent
    {
        DynamicArray<Terrain::WorldChunksOctreeNode> Octree;
        Graphics::BufferHandle OctreeGpuBuffer;
        Graphics::BufferHandle OctreeLeafNodesGpuBuffer;
        onyxF32 RootSize;
        onyxU8 Depth;
    };

    struct VolumeGenerationComponent
    {
        Graphics::ShaderEffectHandle UpdateWorldOctreeShader;
        Graphics::ShaderEffectHandle SetupDispatchGenerateMeshShader;
        Graphics::ShaderEffectHandle GenerateMeshShader;
    };
}

template <>
struct Serialization<Volume::TerrainSettingsComponent>
{
    static bool Serialize(Serializer& serializer, const Volume::TerrainSettingsComponent& volumeTerrain);
    static bool Deserialize(const Deserializer& deserializer, Volume::TerrainSettingsComponent& outVolumeTerrain);
};

}
