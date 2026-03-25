#pragma once

#include <onyx/eventhandler.h>

namespace onyx::volume {

class VolumeBase;
class VolumeChunk;

class VolumeTerrain {
  public:
    void Init( const VolumeBase& volume );
    void Exit();

    void OnPositionChanged( const Vector3f32& position );

    Vector3u32 GetTerrainSize() const { return m_TerrainSize; }
    void SetTerrainSize( const Vector3u32& val ) { m_TerrainSize = val; }

    uint16_t GetChunkSize() const { return m_ChunkSize; }
    void SetChunkSize( uint16_t val ) { m_ChunkSize = val; }

    ONYX_EVENT( Created, VolumeChunk* );

  private:
    uint32_t GetChunk1DIndex( const Vector3u32& index3d );
    Vector3u32 GetChunk3DIndex( uint32_t index );

  private:
    Vector3f32 m_Position = { 0.0f, 0.0f, 0.0f };
    Vector3u32 m_TerrainSize = { 6, 6, 6 };

    uint16_t m_ChunkSize = 2048; // power of 2

    using VolumeChunkPtr = VolumeChunk*;
    std::vector< VolumeChunkPtr > m_VolumeChunks;

    void OnVolumeChunkCreated( VolumeChunkPtr chunk );
};

} // namespace onyx::volume
