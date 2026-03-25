#pragma once

#include <onyx/volume/mesh/meshbuilder.h>

namespace onyx::volume {
namespace CubicalMarchingSquares {
namespace Internal {
template < typename Scalar >
struct LineSegment {
  public:
    LineSegment() = default;
    LineSegment( uint8_t fromEdgeIndex, uint8_t toEdgeIndex )
        : m_FromEdgeIndex( fromEdgeIndex )
        , m_ToEdgeIndex( toEdgeIndex ) {}

    uint8_t GetFromEdgeIndex() const { return m_FromEdgeIndex; }
    uint8_t GetToEdgeIndex() const { return m_ToEdgeIndex; }

    InplaceArray< Vertex, 4 > LinePoints;

  private:
    uint8_t m_FromEdgeIndex = 255;
    uint8_t m_ToEdgeIndex = 255;
};
} // namespace Internal
} // namespace CubicalMarchingSquares
} // namespace onyx::volume