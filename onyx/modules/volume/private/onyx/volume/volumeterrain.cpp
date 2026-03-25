#include <onyx/volume/volumeterrain.h>

#include <onyx/volume/chunk/volumechunk.h>
#include <onyx/volume/dualmarchingcubes/dmcoctreesplitpolicy.h>

namespace onyx::volume {
void VolumeTerrain::Init( const VolumeBase& ) {
    const uint32_t chunkCount = m_TerrainSize[ 0 ] * m_TerrainSize[ 1 ] * m_TerrainSize[ 2 ];
    m_VolumeChunks.reserve( chunkCount );
    for ( uint32_t i = 0; i < chunkCount; ++i ) {
        m_VolumeChunks.emplace_back( new VolumeChunk );
        VolumeChunkPtr& volumeChunk = m_VolumeChunks[ i ];

        Vector3< uint32_t > positon3dIndex = GetChunk3DIndex( i );
        Vector3f32 position( static_cast< float32 >( positon3dIndex[ 0 ] * m_ChunkSize ),
                             static_cast< float32 >( positon3dIndex[ 1 ] * m_ChunkSize ),
                             static_cast< float32 >( positon3dIndex[ 2 ] * m_ChunkSize ) );

        volumeChunk->SetPosition( position );
        volumeChunk->SetSize( m_ChunkSize );

        OnVolumeChunkCreated( volumeChunk );
    }
}

void VolumeTerrain::Exit() {
    for ( VolumeChunk* chunk : m_VolumeChunks ) {
        delete chunk;
    }

    m_VolumeChunks.clear();
}

void VolumeTerrain::OnPositionChanged( const Vector3f32& position ) {
    const float64 lengthSquared = ( position - m_Position ).lengthSquared();

    if ( lengthSquared >= m_ChunkSize * m_ChunkSize ) {
        m_Position = position;

        // load new chunks
        // unload old chunks
    }
}

uint32_t VolumeTerrain::GetChunk1DIndex( const Vector3< uint32_t >& index3d ) {
    return index3d[ 0 ] + m_TerrainSize[ 0 ] * ( index3d[ 1 ] + m_TerrainSize[ 1 ] * index3d[ 2 ] );
}

Vector3u32 VolumeTerrain::GetChunk3DIndex( uint32_t index ) {
    const uint32_t xyMax = m_TerrainSize[ 0 ] * m_TerrainSize[ 1 ];
    const int z = index / xyMax;
    index -= ( z * xyMax );
    const int y = index / m_TerrainSize[ 0 ];
    const int x = index % m_TerrainSize[ 0 ];

    return Vector3u32( x, y, z );
}

void VolumeTerrain::OnVolumeChunkCreated( VolumeChunkPtr /*chunk*/ ) {
    // m_VolumeChunkCreatedSignal.Execute(chunk);
}

} // namespace onyx::volume
