#pragma once

#include <onyx/eventhandler.h>
// #include <onyx/volume/mesh/meshbuilder.h>

namespace onyx::volume {
class MeshBuilder;
namespace CubicalMarchingSquares {
struct HermiteData {
    Vector4f32 Gradient;
};

class VolumeChunk_CMS {
  public:
    VolumeChunk_CMS() = default;
    VolumeChunk_CMS( uint32_t size )
        : m_VolumeData() {
        m_VolumeData.resize( size );
    }

    const Vector3f32& GetPosition() const { return m_Position; }
    void SetPosition( const Vector3f32& position ) { m_Position = position; }

    void SetSize( uint32_t size ) { m_VolumeData.resize( size ); }

    void SetHermiteData( uint64_t index, const Vector4f32& gradient );
    const HermiteData& GetHermiteData( uint64_t index ) const;
    bool IsValid( uint64_t index ) { return index < m_VolumeData.size(); }

    ONYX_EVENT( MeshChanged, const MeshBuilder& );

  private:
    Vector3f32 m_Position{};
    std::vector< HermiteData > m_VolumeData; // z ordered data
};
} // namespace CubicalMarchingSquares
} // namespace onyx::volume
