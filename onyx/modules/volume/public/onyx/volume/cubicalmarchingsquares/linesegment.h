#pragma once

namespace Onyx::Volume
{
    namespace CubicalMarchingSquares
    {
        namespace Internal
        {
            template <typename Scalar>
            struct LineSegment
            {
            private:
                using Vector3T = Vector3<Scalar>;

            public:
                LineSegment() {}
                LineSegment(onyxU8 EdgeIndex, const Vector3T& from, const Vector3T& to)
                    : m_From(from)
                    , m_To(to)
                    , m_EdgeIndex(EdgeIndex)
                {
                }

                const Vector3T& GetFrom() const { return m_From; }
                Vector3T& GetFrom() { return m_From; }

                const Vector3T& GetTo() const { return m_To; }
                Vector3T& GetTo() { return m_To; }

                onyxU8 GetEdgeIndeX() const { return m_EdgeIndex; }
                void SetEdgeIndex(onyxU8 edgeIndex) { m_EdgeIndex = edgeIndex; }

            private:
                Vector3T m_From;
                Vector3T m_To;

                onyxU8 m_EdgeIndex = 255;
            };
        }
    }
}