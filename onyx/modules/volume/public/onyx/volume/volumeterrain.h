#pragma once

#include <onyx/eventhandler.h>

namespace Onyx::Volume
{

class VolumeBase;
class VolumeChunk;

class VolumeTerrain
{
public:
	void Init(const VolumeBase& volume);
	void Exit();

	void OnPositionChanged(const Vector3f& position);

	Vector3<onyxU32> GetTerrainSize() const { return m_TerrainSize; }
	void SetTerrainSize(const Vector3<onyxU32>& val) { m_TerrainSize = val; }

	onyxU16 GetChunkSize() const { return m_ChunkSize; }
	void SetChunkSize(onyxU16 val) { m_ChunkSize = val; }

	ONYX_EVENT(Created, VolumeChunk*);

private:
	onyxU32 GetChunk1DIndex(const Vector3<onyxU32>& index3d);
	Vector3<onyxU32> GetChunk3DIndex(onyxU32 index);

private:
	Vector3f m_Position = { 0.0f, 0.0f, 0.0f };
	Vector3<onyxU32> m_TerrainSize = { 6, 6, 6 };

	onyxU16 m_ChunkSize = 2048; //power of 2

	using VolumeChunkPtr = VolumeChunk*;
	std::vector<VolumeChunkPtr> m_VolumeChunks;

	void OnVolumeChunkCreated(VolumeChunkPtr chunk);
};

}
