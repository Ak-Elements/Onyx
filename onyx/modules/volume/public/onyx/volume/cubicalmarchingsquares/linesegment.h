#pragma once

#include <onyx/volume/mesh/meshbuilder.h>

namespace Onyx::Volume
{
    namespace CubicalMarchingSquares
    {
        namespace Internal
        {
            template <typename Scalar>
            struct LineSegment
            {
            public:
                LineSegment() = default;
                LineSegment(onyxU8 fromEdgeIndex, onyxU8 toEdgeIndex)
                    : m_FromEdgeIndex(fromEdgeIndex)
                    , m_ToEdgeIndex(toEdgeIndex)
                {
                }

                onyxU8 GetFromEdgeIndex() const { return m_FromEdgeIndex; }
                onyxU8 GetToEdgeIndex() const { return m_ToEdgeIndex; }

                InplaceArray<Vertex, 4> LinePoints;
            private:
                onyxU8 m_FromEdgeIndex = 255;
                onyxU8 m_ToEdgeIndex = 255;
            };
        }
    }
}