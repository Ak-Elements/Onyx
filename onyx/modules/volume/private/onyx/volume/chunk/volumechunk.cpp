#include <onyx/volume/chunk/volumechunk.h>

#include <onyx/volume/isosurface/isosurface.h>
#include <onyx/volume/octree/octree.h>
#include <onyx/volume/octree/octreenode.h>

namespace Onyx::Volume
{
    VolumeChunk::VolumeChunk() = default;
    VolumeChunk::~VolumeChunk() = default;

    void VolumeChunk::Reset()
    {
        m_VolumeOctree.GetRootNode().Merge();
    }

    void VolumeChunk::Load(IsoSurfaceMethod isoSurfaceMethod, onyxU8 maxOctreeLevel, onyxF32 octreeRootSize, onyxF32 maxGeometricError, onyxF32 sampleResolution, onyxF32 complexSurfaceThreshold, onyxF32 maxDistanceSkirts, const VolumeBase& volumeBase)
    {
        VolumeChunckLoadRequestData loadRequestData(isoSurfaceMethod, m_Position, maxOctreeLevel, octreeRootSize, maxGeometricError, sampleResolution, complexSurfaceThreshold, volumeBase);
        loadRequestData.m_MaxDistanceSkirts = maxDistanceSkirts;
        m_ChunkLoader.RequestLoad(loadRequestData, [this](const VolumeChunckLoadRequestData& reqData) { m_MeshChanged(reqData.m_MeshBuilder); });
    }
}
