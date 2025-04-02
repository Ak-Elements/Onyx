#pragma once

#include <onyx/volume/mesh/meshbuilder.h>

#include <onyx/volume/octree/octree.h>
#include <onyx/volume/dualgrid/dualgrid.h>

#include <onyx/volume/chunk/volumechunkloader.h>
#include <onyx/eventhandler.h>

#include <onyx/volume/isosurface/isosurface.h>

namespace Onyx::Volume
{

struct VolumeDataContainer
{
    VolumeOctreeNodeMetaData MetaData = VolumeOctreeNodeMetaData::None;
    Vector4f Gradient;
	Vector3f AnalyticalNormal;
    Vector3f Position; // position and half extent
    onyxF32 HalfExtent = 0.0f;
};

class VolumeChunk
{
public:
    VolumeChunk();
    ~VolumeChunk();

	const Vector3f& GetPosition() const { return m_Position; }
	void SetPosition(const Vector3f& position) { m_Position = position; }

	onyxF32 GetSize() const { return m_Size; }
	void SetSize(onyxF32 size) { m_Size = size; }

    void Reset();
    VolumeChunkOctree& GetOctree() { return m_VolumeOctree; }
    VolumeChunkDualgrid& GetDualgrid() { return m_VolumeDualgrid; }

    //debug
    void Load(IsoSurfaceMethod isoSurfaceMethod, onyxU8 maxOctreeLevel, onyxF32 octreeRootSize, onyxF32 maxGeometricError, onyxF32 sampleResolution, onyxF32 complexSurfaceThreshold, onyxF32 maxDistanceSkirts, const VolumeBase& volumeBase);

    ONYX_EVENT(MeshChanged, const MeshBuilder&);

private:
	Vector3f m_Position;
	onyxF32 m_Size = 0;
    
    VolumeChunkOctree m_VolumeOctree;
    VolumeChunkDualgrid m_VolumeDualgrid;

    VolumeChunkLoader m_ChunkLoader;
};

}
