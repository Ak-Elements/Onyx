#pragma once

#include <onyx/volume/mesh/meshbuilder.h>

#include <onyx/volume/dualgrid/dualgrid.h>
#include <onyx/volume/octree/octree.h>

#include <onyx/eventhandler.h>
#include <onyx/volume/chunk/volumechunkloader.h>

#include <onyx/volume/isosurface/isosurface.h>

namespace onyx::volume {
struct VolumeDataContainer {
    VolumeOctreeNodeMetaData MetaData = VolumeOctreeNodeMetaData::None;
    Vector4f32 Gradient;
    Vector3f32 AnalyticalNormal;
    Vector3f32 Position; // position and half extent
    float32 HalfExtent = 0.0f;
};

class VolumeChunk {
  public:
    using VolumeChunkOctree = Octree< UniquePtr< VolumeDataContainer >, uint32_t >;
    using VolumeChunkDualgrid = Dualgrid< OctreeNode< UniquePtr< VolumeDataContainer > > >;

    VolumeChunk();
    ~VolumeChunk();

    const Vector3f32& GetPosition() const { return m_Position; }
    void SetPosition( const Vector3f32& position ) { m_Position = position; }

    float32 GetSize() const { return m_Size; }
    void SetSize( float32 size ) { m_Size = size; }

    void Reset();
    VolumeChunkOctree& GetOctree() { return m_VolumeOctree; }
    VolumeChunkDualgrid& GetDualgrid() { return m_VolumeDualgrid; }

    // debug
    void Load( IsoSurfaceMethod isoSurfaceMethod,
               uint8_t maxOctreeLevel,
               float32 octreeRootSize,
               float32 maxGeometricError,
               float32 sampleResolution,
               float32 complexSurfaceThreshold,
               float32 maxDistanceSkirts,
               const VolumeBase& volumeBase );

    ONYX_EVENT( MeshChanged, const MeshBuilder& );

  private:
    Vector3f32 m_Position;
    float32 m_Size = 0;

    VolumeChunkOctree m_VolumeOctree;
    VolumeChunkDualgrid m_VolumeDualgrid;

    VolumeChunkLoader m_ChunkLoader;
};

} // namespace onyx::volume
