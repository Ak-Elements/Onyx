#pragma once

#include <onyx/container/inplacearray.h>

namespace Onyx
{
    namespace SAT_2D
    {
        template <typename Scalar>
        struct Projection
        {
        public:
            Projection(Scalar min, Scalar max)
                : m_Min(min)
                , m_Max(max)
            {
            }

            Scalar m_Min;
            Scalar m_Max;
        };

        template <typename Scalar>
        struct Triangle
        {
        public:
            Triangle(const Vector2<Scalar>& p0, const Vector2<Scalar>& p1, const Vector2<Scalar>& p2)
                : m_Point0(p0)
                , m_Point1(p1)
                , m_Point2(p2)
            {
            }

            InplaceArray<Vector2<Scalar>, 3> GetEdgeNormals() const
            {
                Vector2<Scalar> edge0 = m_Point1 - m_Point0;
                Vector2<Scalar> edge1 = m_Point2 - m_Point1;
                Vector2<Scalar> edge2 = m_Point0 - m_Point2;

                InplaceArray<Vector2<Scalar>, 3> edgeNormals;
                edgeNormals.Add(edge0.GetPerpendicularClockwise());
                edgeNormals.Add(edge1.GetPerpendicularClockwise());
                edgeNormals.Add(edge2.GetPerpendicularClockwise());

                return edgeNormals;
            }

            const Vector2<Scalar>& GetPoint0() const { return m_Point0; }
            const Vector2<Scalar>& GetPoint1() const { return m_Point1; }
            const Vector2<Scalar>& GetPoint2() const { return m_Point2; }

        private:
            Vector2<Scalar> m_Point0;
            Vector2<Scalar> m_Point1;
            Vector2<Scalar> m_Point2;
        };

        template <typename Scalar>
        struct Line
        {
        public:
            Line(const Vector2<Scalar>& p0, const Vector2<Scalar>& p1)
                : m_Point0(p0)
                , m_Point1(p1)
            {
            }

            Vector2<Scalar> GetNormal() const
            {
                Vector2<Scalar> line = m_Point1 - m_Point0;
                return line.GetPerpendicularClockwise();
            }

            const Vector2<Scalar>& GetPoint0() const { return m_Point0; }
            const Vector2<Scalar>& GetPoint1() const { return m_Point1; }

        private:
            Vector2<Scalar> m_Point0;
            Vector2<Scalar> m_Point1;
        };


        template <typename Scalar>
        static bool IsOverlapping(const Vector2<Scalar>& triangle1Point0, const Vector2<Scalar>& triangle1Point1, const Vector2<Scalar>& triangle1Point2,
            const Vector2<Scalar>& linePoint0, const Vector2<Scalar>& linePoint1)
        {
            Triangle<Scalar> tri(triangle1Point0, triangle1Point1, triangle1Point2);
            Line<Scalar> line(linePoint0, linePoint1);

            const InplaceArray<Vector2<Scalar>, 3> & triangle1EdgeNormals = tri.GetEdgeNormals();

            for (const Vector2<Scalar>& edgeNormal : triangle1EdgeNormals)
            {
                const Projection<Scalar>& p1 = GetProjection(tri, edgeNormal);
                const Projection<Scalar>& p2 = GetProjection(line, edgeNormal);

                const bool hasNoOverlap = (p1.m_Max < p2.m_Min) || (p2.m_Max < p1.m_Min);
                if (hasNoOverlap)
                {
                    return false;
                }
            }

            // test the line
            const Vector2<Scalar> lineNormal = line.GetNormal();
            const Projection<Scalar>& p1 = GetProjection(tri, lineNormal);
            const Projection<Scalar>& p2 = GetProjection(line, lineNormal);

            const bool hasNoOverlap = (p1.m_Max < p2.m_Min) || (p2.m_Max < p1.m_Min);
            return !hasNoOverlap;
        }

        template <typename Scalar>
        static bool IsOverlapping(const Vector2<Scalar>& triangle1Point0, const Vector2<Scalar>& triangle1Point1, const Vector2<Scalar>& triangle1Point2,
                                  const Vector2<Scalar>& triangle2Point0, const Vector2<Scalar>& triangle2Point1, const Vector2<Scalar>& triangle2Point2)
        {
            Triangle<Scalar> tri1(triangle1Point0, triangle1Point1, triangle1Point2);
            Triangle<Scalar> tri2(triangle2Point0, triangle2Point1, triangle2Point2);

            const InplaceArray<Vector2<Scalar>, 3>& triangle1EdgeNormals = tri1.GetEdgeNormals();

            for (const Vector2<Scalar>& edgeNormal : triangle1EdgeNormals)
            {
                const Projection<Scalar>& p1 = GetProjection(tri1, edgeNormal);
                const Projection<Scalar>& p2 = GetProjection(tri2, edgeNormal);

                const bool hasNoOverlap = (p1.m_Max < p2.m_Min) || (p2.m_Max < p1.m_Min);
                if (hasNoOverlap)
                {
                    return false;
                }
            }

            const InplaceArray<Vector2<Scalar>, 3> & triangle2EdgeNormals = tri2.GetEdgeNormals();
            for (const Vector2<Scalar>& edgeNormal : triangle2EdgeNormals)
            {
                const Projection<Scalar>& p1 = GetProjection(tri1, edgeNormal);
                const Projection<Scalar>& p2 = GetProjection(tri2, edgeNormal);

                const bool hasNoOverlap = (p1.m_Max < p2.m_Min) || (p2.m_Max < p1.m_Min);
                if (hasNoOverlap)
                {
                    return false;
                }
            }

            return true;
        }

        template <typename Scalar>
        static Projection<Scalar> GetProjection(const Triangle<Scalar>& triangle, const Vector2<Scalar>& edgeNormal)
        {
            auto min = triangle.GetPoint0() | edgeNormal;
            auto max = min;

            auto p = triangle.GetPoint1() | edgeNormal;
            if (p < min)
            {
                min = p;
            }
            else if (p > max)
            {
                max = p;
            }

            p = triangle.GetPoint2() | edgeNormal;
            if (p < min)
            {
                min = p;
            }
            else if (p > max)
            {
                max = p;
            }

            return Projection<Scalar>(static_cast<onyxF32>(min), static_cast<onyxF32>(max));
        }

        template <typename Scalar>
        static Projection<Scalar> GetProjection(const Line<Scalar>& line, const Vector2<Scalar>& edgeNormal)
        {
            auto min = line.GetPoint0() | edgeNormal;
            auto max = min;

            auto p = line.GetPoint1() | edgeNormal;
            if (p < min)
            {
                min = p;
            }
            else if (p > max)
            {
                max = p;
            }

            return Projection<Scalar>(static_cast<onyxF32>(min), static_cast<onyxF32>(max));
        }
    }

}
