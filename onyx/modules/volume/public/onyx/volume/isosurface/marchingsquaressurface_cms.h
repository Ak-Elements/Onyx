#pragma once


#include <onyx/container/inplacearray.h>
#include <onyx/volume/isosurface/marchingsquarestable_cms.h>
#include <onyx/volume/cubicalmarchingsquares/linesegment.h>
#include <onyx/geometry/sat.h>
#include <onyx/volume/cubicalmarchingsquares/vertex.h>

namespace Onyx::Volume
{
    namespace CubicalMarchingSquares
    {

        template <typename Scalar>
        class MarchingSquares
        {
        private:
            using Vector2T = Vector2<Scalar>;
            using Vector3T = Vector3<Scalar>;
            using Vector4T = Vector4<Scalar>;
            using LineSegment = Internal::LineSegment<Scalar>;
            using Vertex = Internal::Vertex<Scalar>;
        public:
            static constexpr onyxF32 ISO_LEVEL = 0.0f;

            explicit MarchingSquares(const Scalar maxDistance)
                : m_MaxDistance(maxDistance)
            {
            }

            Scalar minX;
            Scalar minY;
            Scalar maxX;
            Scalar maxY;

            void GenerateLines(onyxU8 faceIndex, const InplaceArray<Vector3T, 4> & corners, const InplaceArray<Vector4T, 4>& hermiteData, InplaceArray<LineSegment, 24>& outLineSegments)
            {
                /*
                DEBUG
                */

                onyxU8 xIndex = MarchingSquares_CMS::FACE_COORDS[faceIndex][0];
                onyxU8 yIndex = MarchingSquares_CMS::FACE_COORDS[faceIndex][1];

                Vector2T corner0_2D(corners[0][xIndex], corners[0][yIndex]);
                Vector2T corner1_2D(corners[1][xIndex], corners[1][yIndex]);
                Vector2T corner2_2D(corners[2][xIndex], corners[2][yIndex]);
                Vector2T corner3_2D(corners[3][xIndex], corners[3][yIndex]);

                minX = minY = std::numeric_limits<Scalar>::max();
                maxX = maxY = std::numeric_limits<Scalar>::min();

                for (onyxU8 i = 0; i < 4; ++i)
                {
                    if (corners[i][xIndex] < minX)
                    {
                        minX = corners[i][xIndex];
                    }

                    if (corners[i][yIndex] < minY)
                    {
                        minY = corners[i][yIndex];
                    }

                    if (corners[i][xIndex] > maxX)
                    {
                        maxX = corners[i][xIndex];
                    }

                    if (corners[i][yIndex] > maxY)
                    {
                        maxY = corners[i][yIndex];
                    }
                }

                onyxU8 msCase =
                    (hermiteData[0][3] >= ISO_LEVEL ? 1 : 0) |
                    (hermiteData[1][3] >= ISO_LEVEL ? 2 : 0) |
                    (hermiteData[2][3] >= ISO_LEVEL ? 4 : 0) |
                    (hermiteData[3][3] >= ISO_LEVEL ? 8 : 0);

                if ((msCase == 0) || (msCase == 15))
                {
                    return;
                }

                // Don't generate lines if we are completely inside and far enough away from the surface
                /*if ((msCase == 15) &&
                    (hermiteData[0][3] >= m_MaxDistance) &&
                    (hermiteData[1][3] >= m_MaxDistance) &&
                    (hermiteData[2][3] >= m_MaxDistance) &&
                    (hermiteData[3][3] >= m_MaxDistance))
                {
                    return;
                }*/

                if (MarchingSquares_CMS::MS_TABLE[msCase][2] >= 0) // ambigous case
                {
                    ResolveAmbigousCase(faceIndex, msCase, corners, hermiteData, outLineSegments);
                }
                else
                {
                    onyxU8 edge0_A = MarchingSquares_CMS::MS_TABLE[msCase][0];
                    onyxU8 edge0_B = MarchingSquares_CMS::MS_TABLE[msCase][1];

                    Vertex Edge0_FromVertex;
                    Vertex Edge0_ToVertex;

                    CalculateZeroCrossing(edge0_A, corners, hermiteData, Edge0_FromVertex);
                    CalculateZeroCrossing(edge0_B, corners, hermiteData, Edge0_ToVertex);

                    Vector3T intersection;
                    if (ResolveFaceSharpFeature(faceIndex, Edge0_FromVertex, Edge0_ToVertex, intersection))
                    {
                        outLineSegments.Add({ edge0_A, Edge0_FromVertex.Position, intersection});
                        outLineSegments.Add({ edge0_B, Edge0_ToVertex.Position, intersection });
                    }
                    else
                    {
                        outLineSegments.Add({ edge0_A, Edge0_FromVertex.Position, Edge0_ToVertex.Position });
                    }
                }
            }

        private:
            void CalculateZeroCrossing(const onyxU8 edgeIndex, const InplaceArray<Vector3T, 4> & corners, const InplaceArray<Vector4T, 4> & hermiteData, Vertex& outVertex) const
            {
                const uint8_t v0 = MarchingSquares_CMS::VERTEX_MAP[edgeIndex][0];
                const uint8_t v1 = MarchingSquares_CMS::VERTEX_MAP[edgeIndex][1];

                const onyxF32 isoVal0 = hermiteData[v0][3];
                const onyxF32 isoVal1 = hermiteData[v1][3];

                // linear interpolation to get the point on the surface and the normal
                const Scalar interpolated = (ISO_LEVEL - isoVal0) / (isoVal1 - isoVal0);

                outVertex.Position = corners[v0] + interpolated * (corners[v1] - corners[v0]);
                outVertex.Normal = Vector3T(hermiteData[v0] + (hermiteData[v1] - hermiteData[v0]) * interpolated);
                //outVertex.Normal.Normalize();
            }

            void ResolveAmbigousCase(onyxU8 faceIndex,
                                     onyxU8 msCase,
                                     const InplaceArray<Vector3T, 4> & corners,
                                     const InplaceArray<Vector4T, 4> & hermiteData,
                                     InplaceArray<LineSegment, 24> & outLineSegments)
            {
                onyxU8 edge0_A = MarchingSquares_CMS::MS_TABLE[msCase][0];
                onyxU8 edge0_B = MarchingSquares_CMS::MS_TABLE[msCase][1];

                Vertex Edge0_FromVertex;
                Vertex Edge0_ToVertex;

                CalculateZeroCrossing(edge0_A, corners, hermiteData, Edge0_FromVertex);
                CalculateZeroCrossing(edge0_B, corners, hermiteData, Edge0_ToVertex);

                onyxU8 edge1_A = MarchingSquares_CMS::MS_TABLE[msCase][2];
                onyxU8 edge1_B = MarchingSquares_CMS::MS_TABLE[msCase][3];

                Vertex Edge1_FromVertex;
                Vertex Edge1_ToVertex;

                CalculateZeroCrossing(edge1_A, corners, hermiteData, Edge1_FromVertex);
                CalculateZeroCrossing(edge1_B, corners, hermiteData, Edge1_ToVertex);

                // is overlapping?
                if (HasFaceAmbiguity(faceIndex, msCase, Edge0_FromVertex, Edge0_ToVertex, Edge1_FromVertex, Edge1_ToVertex))
                {
                    edge0_A = MarchingSquares_CMS::MS_TABLE_AMBIGUOUS[msCase][0];
                    edge0_B = MarchingSquares_CMS::MS_TABLE_AMBIGUOUS[msCase][1];

                    CalculateZeroCrossing(edge0_A, corners, hermiteData, Edge0_FromVertex);
                    CalculateZeroCrossing(edge0_B, corners, hermiteData, Edge0_ToVertex);

                    edge1_A = MarchingSquares_CMS::MS_TABLE_AMBIGUOUS[msCase][2];
                    edge1_B = MarchingSquares_CMS::MS_TABLE_AMBIGUOUS[msCase][3];

                    CalculateZeroCrossing(edge1_A, corners, hermiteData, Edge1_FromVertex);
                    CalculateZeroCrossing(edge1_B, corners, hermiteData, Edge1_ToVertex);
                }

                // we calculate the intersection now 2 times in case of no ambiguity
                Vector3T intersection;
                if (ResolveFaceSharpFeature(faceIndex, Edge0_FromVertex, Edge0_ToVertex, intersection))
                {
                    outLineSegments.Add({ edge0_A, Edge0_FromVertex.Position, intersection });
                    outLineSegments.Add({ edge0_B, Edge0_ToVertex.Position, intersection });
                }
                else
                {
                    outLineSegments.Add({ edge0_A, Edge0_FromVertex.Position, Edge0_ToVertex.Position });
                }

                Vector3T intersection2;
                if (ResolveFaceSharpFeature(faceIndex, Edge1_FromVertex, Edge1_ToVertex, intersection2))
                {
                    outLineSegments.Add({ edge1_A, Edge1_FromVertex.Position, intersection2 });
                    outLineSegments.Add({ edge1_B, Edge1_ToVertex.Position, intersection2 });
                }
                else
                {
                    outLineSegments.Add({ edge1_A, Edge1_FromVertex.Position, Edge1_ToVertex.Position });
                }
            }

            bool ResolveFaceSharpFeature(onyxU8 faceIndex, const Vertex& edgeCrossing_From, const Vertex& edgeCrossing_To, Vector3T& outIntersectionPosition)
            {
                onyxU8 xIndex = MarchingSquares_CMS::FACE_COORDS[faceIndex][0];
                onyxU8 yIndex = MarchingSquares_CMS::FACE_COORDS[faceIndex][1];

                Vector2T edgeCrossingFromPosition(edgeCrossing_From.Position[xIndex], edgeCrossing_From.Position[yIndex]);
                Vector2T edgeCrossingFromNormal(edgeCrossing_From.Normal[xIndex], edgeCrossing_From.Normal[yIndex]);
                edgeCrossingFromNormal.Normalize();

                Vector2T edgeCrossingToPosition(edgeCrossing_To.Position[xIndex], edgeCrossing_To.Position[yIndex]);
                Vector2T edgeCrossingToNormal(edgeCrossing_To.Normal[xIndex], edgeCrossing_To.Normal[yIndex]);
                edgeCrossingToNormal.Normalize();
                Vector2T intersectionPoint;
                const bool hasSharpFeature = ResolveFaceSharpFeature(edgeCrossingFromPosition, edgeCrossingFromNormal, edgeCrossingToPosition, edgeCrossingToNormal, intersectionPoint);
                if (hasSharpFeature)
                {
                    outIntersectionPosition = edgeCrossing_From.Position;
                    outIntersectionPosition[xIndex] = intersectionPoint[0];
                    outIntersectionPosition[yIndex] = intersectionPoint[1];
                }

                return hasSharpFeature;
            }

            bool ResolveFaceSharpFeature(const Vector2T& edgeCrossingFromPosition,
                                        const Vector2T& edgeCrossingFromNormal,
                                        const Vector2T& edgeCrossingToPosition,
                                        const Vector2T& edgeCrossingToNormal,
                                        Vector2T& outIntersectionPosition)
            {
                const bool hasSharpFeature = HasSharpFeature(edgeCrossingFromNormal, edgeCrossingToNormal);
                if (hasSharpFeature)
                {
                    outIntersectionPosition = ComputeIntersection(edgeCrossingFromPosition, edgeCrossingFromNormal, edgeCrossingToPosition, edgeCrossingToNormal);

                    // clamp to min and max of cell boundaries
                    if (outIntersectionPosition[0] < minX || outIntersectionPosition[0] > maxX)
                    {
                        outIntersectionPosition[0] = std::max(minX, std::min(outIntersectionPosition[0], maxX));
                    }

                    if (outIntersectionPosition[1] < minY || outIntersectionPosition[1] > maxY)
                    {
                        outIntersectionPosition[1] = std::max(minY, std::min(outIntersectionPosition[1], maxY));
                    }
                }

                return hasSharpFeature;
            }

            bool HasFaceAmbiguity(onyxU8 faceIndex, onyxU8 /*msCase*/, const Vertex& edgeCrossing0_From, const Vertex& edgeCrossing0_To, const Vertex& edgeCrossing1_From, const Vertex& edgeCrossing1_To)
            {
                onyxU8 xIndex = MarchingSquares_CMS::FACE_COORDS[faceIndex][0];
                onyxU8 yIndex = MarchingSquares_CMS::FACE_COORDS[faceIndex][1];

                m_Intersection0P0 = edgeCrossing0_From.Position;
                m_Intersection0P1 = edgeCrossing0_From.Position;
                m_Intersection0P2 = edgeCrossing0_To.Position;

                m_Intersection1P0 = edgeCrossing1_From.Position;
                m_Intersection1P1 = edgeCrossing1_From.Position;
                m_Intersection1P2 = edgeCrossing1_To.Position;

                Vector2T edgeCrossing0FromPosition(edgeCrossing0_From.Position[xIndex], edgeCrossing0_From.Position[yIndex]);
                Vector2T edgeCrossing0FromNormal(edgeCrossing0_From.Normal[xIndex], edgeCrossing0_From.Normal[yIndex]);
                edgeCrossing0FromNormal.Normalize();

                Vector2T edgeCrossing0ToPosition(edgeCrossing0_To.Position[xIndex], edgeCrossing0_To.Position[yIndex]);
                Vector2T edgeCrossing0ToNormal(edgeCrossing0_To.Normal[xIndex], edgeCrossing0_To.Normal[yIndex]);
                edgeCrossing0ToNormal.Normalize();

                Vector2T edgeCrossing1FromPosition(edgeCrossing1_From.Position[xIndex], edgeCrossing1_From.Position[yIndex]);
                Vector2T edgeCrossing1FromNormal(edgeCrossing1_From.Normal[xIndex], edgeCrossing1_From.Normal[yIndex]);
                edgeCrossing1FromNormal.Normalize();

                Vector2T edgeCrossing1ToPosition(edgeCrossing1_To.Position[xIndex], edgeCrossing1_To.Position[yIndex]);
                Vector2T edgeCrossing1ToNormal(edgeCrossing1_To.Normal[xIndex], edgeCrossing1_To.Normal[yIndex]);
                edgeCrossing1ToNormal.Normalize();

                Vector2T intersectionEdge0;
                Vector2T intersectionEdge1;

                const bool hasSharpFeatureEdge0 = ResolveFaceSharpFeature(edgeCrossing0FromPosition, edgeCrossing0FromNormal, edgeCrossing0ToPosition, edgeCrossing0ToNormal, intersectionEdge0);
                const bool hasSharpFeatureEdge1 = ResolveFaceSharpFeature(edgeCrossing1FromPosition, edgeCrossing1FromNormal, edgeCrossing1ToPosition, edgeCrossing1ToNormal, intersectionEdge1);

                hasIntersection0 = hasSharpFeatureEdge0;
                hasIntersection1 = hasSharpFeatureEdge1;

                if (hasSharpFeatureEdge0 && hasSharpFeatureEdge1)
                {
                    m_Intersection0P0[xIndex] = intersectionEdge0[0];
                    m_Intersection0P0[yIndex] = intersectionEdge0[1];

                    m_Intersection0P1[xIndex] = edgeCrossing0FromPosition[0];
                    m_Intersection0P1[yIndex] = edgeCrossing0FromPosition[1];

                    m_Intersection0P2[xIndex] = edgeCrossing0ToPosition[0];
                    m_Intersection0P2[yIndex] = edgeCrossing0ToPosition[1];

                    m_Intersection1P0[xIndex] = intersectionEdge1[0];
                    m_Intersection1P0[yIndex] = intersectionEdge1[1];

                    m_Intersection1P1[xIndex] = edgeCrossing1FromPosition[0];
                    m_Intersection1P1[yIndex] = edgeCrossing1FromPosition[1];

                    m_Intersection1P2[xIndex] = edgeCrossing1ToPosition[0];
                    m_Intersection1P2[yIndex] = edgeCrossing1ToPosition[1];

                    if (Onyx::SAT_2D::IsOverlapping(edgeCrossing0FromPosition, edgeCrossing0ToPosition, intersectionEdge0, edgeCrossing1FromPosition, edgeCrossing1ToPosition, intersectionEdge1))
                    {
                        // overlap choose other case
                        return true;
                    }
                }
                else if (hasSharpFeatureEdge0)
                {
                    m_Intersection0P0[xIndex] = intersectionEdge0[0];
                    m_Intersection0P0[yIndex] = intersectionEdge0[1];

                    m_Intersection0P1[xIndex] = edgeCrossing0FromPosition[0];
                    m_Intersection0P1[yIndex] = edgeCrossing0FromPosition[1];
                    
                    m_Intersection0P2[xIndex] = edgeCrossing0ToPosition[0];
                    m_Intersection0P2[yIndex] = edgeCrossing0ToPosition[1];

                    if (Onyx::SAT_2D::IsOverlapping(edgeCrossing0FromPosition, intersectionEdge0, edgeCrossing0ToPosition, edgeCrossing1FromPosition, edgeCrossing1ToPosition))
                    {
                        return true;
                    }
                }
                else if (hasSharpFeatureEdge1)
                {
                    m_Intersection1P0[xIndex] = intersectionEdge1[0];
                    m_Intersection1P0[yIndex] = intersectionEdge1[1];

                    m_Intersection1P1[xIndex] = edgeCrossing1FromPosition[0];
                    m_Intersection1P1[yIndex] = edgeCrossing1FromPosition[1];

                    m_Intersection1P2[xIndex] = edgeCrossing1ToPosition[0];
                    m_Intersection1P2[yIndex] = edgeCrossing1ToPosition[1];

                    if (Onyx::SAT_2D::IsOverlapping(edgeCrossing1FromPosition, intersectionEdge1, edgeCrossing1ToPosition, edgeCrossing0FromPosition, edgeCrossing0ToPosition))
                    {
                        return true;
                    }
                }

                return false;
            }

            Vector2T ComputeIntersection(const Vector2T& edgeCrossingPoint0, const Vector2T& edgeCrossingNormal0, const Vector2T& edgeCrossingPoint1, const Vector2T& edgeCrossingNormal1)
            {
                const Vector2T direction = edgeCrossingPoint1 - edgeCrossingPoint0;
                const Vector2T d1(edgeCrossingNormal1[1], -edgeCrossingNormal1[0]); // perpendicular to edgeCrossingNormal1
                const auto proj = (-edgeCrossingNormal0 | direction) / (edgeCrossingNormal0 | d1);
                return edgeCrossingPoint1 + d1 * static_cast<onyxF32>(proj);
            }

            inline bool HasSharpFeature(const Vector2T& edgeCrossingNormal0, const Vector2T& edgeCrossingNormal1)
            {
                auto dot = edgeCrossingNormal0.Dot(-edgeCrossingNormal1);
                auto cross = edgeCrossingNormal0.Cross(-edgeCrossingNormal1);
                static const onyxF64 SHARP_ANGLE_FEATURE = cos((135 * (std::numbers::pi_v<onyxF32> / 180.0))); /*Degrees*/
                return (std::abs(cross) > std::numeric_limits<Scalar>::epsilon()) && dot >= SHARP_ANGLE_FEATURE;
            }
        public:
            Vector3T m_Intersection0P0;
            Vector3T m_Intersection0P1;
            Vector3T m_Intersection0P2;

            Vector3T m_Intersection1P0;
            Vector3T m_Intersection1P1;
            Vector3T m_Intersection1P2;

            bool hasIntersection0 = false;
            bool hasIntersection1 = false;

        private:
            const Scalar m_MaxDistance = {};
        };
    }
}
