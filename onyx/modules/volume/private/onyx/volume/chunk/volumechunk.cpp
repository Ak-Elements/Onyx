#include <onyx/volume/chunk/volumechunk.h>
#include <onyx/volume/chunk/volumechunkloadrequest.h>

#include <onyx/volume/isosurface/isosurface.h>
#include <onyx/volume/octree/octree.h>
#include <onyx/volume/octree/octreenode.h>

namespace onyx::volume {
VolumeChunk::VolumeChunk() = default;
VolumeChunk::~VolumeChunk() = default;

void VolumeChunk::Reset() {
    m_VolumeOctree.GetRootNode().Merge();
}

void VolumeChunk::Load( IsoSurfaceMethod isoSurfaceMethod,
                        uint8_t maxOctreeLevel,
                        float32 octreeRootSize,
                        float32 maxGeometricError,
                        float32 sampleResolution,
                        float32 complexSurfaceThreshold,
                        float32 maxDistanceSkirts,
                        const VolumeBase& volumeBase ) {
    VolumeChunckLoadRequestData loadRequestData( isoSurfaceMethod,
                                                 m_Position,
                                                 maxOctreeLevel,
                                                 octreeRootSize,
                                                 maxGeometricError,
                                                 sampleResolution,
                                                 complexSurfaceThreshold,
                                                 volumeBase );
    loadRequestData.m_MaxDistanceSkirts = maxDistanceSkirts;
    m_ChunkLoader.RequestLoad( loadRequestData, [ this ]( const VolumeChunckLoadRequestData& reqData ) {
        m_MeshChanged( reqData.m_MeshBuilder );
    } );
}
} // namespace onyx::volume
