#pragma once

#include <onyx/volume/octree/octreesplitpolicy.h>

namespace Onyx::Volume
{

template <typename Scalar>
class CMSOctreeSplitPolicy : public OctreeSplitPolicy<Scalar>
{
private:
    using super = OctreeSplitPolicy<Scalar>;
public:
    CMSOctreeSplitPolicy(onyxU8 maxOctreeLevel, Scalar octreeRootSize, Scalar maxGeometricError, Scalar complexSurfaceThreshold, const VolumeBase& volumeBase)
        : super(maxOctreeLevel, octreeRootSize, volumeBase)
        , m_MaxGeometricError(maxGeometricError)
        , m_ComplexSurfaceThreshold(complexSurfaceThreshold)
    {
    }

    bool ShouldSplit(VolumeChunkOctree::OctreeNodeT& node, const Vector3<Scalar>& nodeWorldPosition, Scalar halfExtent, onyxU8 nodeLevel)
    {
        using std::abs;

        if (nodeLevel > super::m_MaxOctreeLevel)
        {
            return false;
        }

        const VolumeBase& volumeBase = *(super::m_VolumeSource);

        Vector4<Scalar> centerValue = volumeBase.GetValueAndGradient(nodeWorldPosition);

        const Vector3<Scalar> halfExtentVec(halfExtent);
        Vector3<Scalar> corner0(nodeWorldPosition - halfExtentVec);
        Vector3<Scalar> corner6(nodeWorldPosition + halfExtentVec);

        if (abs(centerValue[3]) > (corner6 - corner0).Length())
        {
            // set value
            node.GetData()->Gradient = centerValue;
            return false;
        }

        const Vector3<Scalar> corners[8] =
        {
            corner0,
            Vector3<Scalar>(nodeWorldPosition[0] + halfExtent, nodeWorldPosition[1] - halfExtent, nodeWorldPosition[2] - halfExtent), // corner1
            Vector3<Scalar>(nodeWorldPosition[0] + halfExtent, nodeWorldPosition[1] - halfExtent, nodeWorldPosition[2] + halfExtent), // corner2
            Vector3<Scalar>(nodeWorldPosition[0] - halfExtent, nodeWorldPosition[1] - halfExtent, nodeWorldPosition[2] + halfExtent), // corner3
            Vector3<Scalar>(nodeWorldPosition[0] - halfExtent, nodeWorldPosition[1] + halfExtent, nodeWorldPosition[2] - halfExtent), // corner4
            Vector3<Scalar>(nodeWorldPosition[0] + halfExtent, nodeWorldPosition[1] + halfExtent, nodeWorldPosition[2] - halfExtent), // corner5
            corner6,
            Vector3<Scalar>(nodeWorldPosition[0] - halfExtent, nodeWorldPosition[1] + halfExtent, nodeWorldPosition[2] + halfExtent), // corner7

        };

        const Vector4<Scalar> values[8] =
        {
            volumeBase.GetValueAndGradient(corners[0]),
            volumeBase.GetValueAndGradient(corners[1]),
            volumeBase.GetValueAndGradient(corners[2]),
            volumeBase.GetValueAndGradient(corners[3]),
            volumeBase.GetValueAndGradient(corners[4]),
            volumeBase.GetValueAndGradient(corners[5]),
            volumeBase.GetValueAndGradient(corners[6]),
            volumeBase.GetValueAndGradient(corners[7])
        };

        if (HasComplexSurface(values))
        {
            return true;
        }

        if (ExceedsGeometricError(corners, values, nodeWorldPosition))
        {
            return true;
        }

        node.GetData()->Gradient = centerValue;
        return false;
    }

private:
    bool ExceedsGeometricError(const Vector3<Scalar> (&corners)[8], const Vector4<Scalar> (&cornerValues)[8], const Vector3<Scalar>& nodeCenter)
    {
        using std::abs;
        using std::numeric_limits;

        // Error metric of http://www.andrew.cmu.edu/user/jessicaz/publication/meshing/
        const onyxF32 f000 = cornerValues[0][3];
        const onyxF32 f001 = cornerValues[1][3];
        const onyxF32 f010 = cornerValues[2][3];
        const onyxF32 f011 = cornerValues[3][3];
        const onyxF32 f100 = cornerValues[4][3];
        const onyxF32 f101 = cornerValues[5][3];
        const onyxF32 f110 = cornerValues[6][3];
        const onyxF32 f111 = cornerValues[7][3];

        const Vector3<Scalar>& from = corners[0];
        const Vector3<Scalar>& to = corners[6];

        const Vector3<Scalar> centerBackBottom(nodeCenter[0], from[1], from[2]);
        const Vector3<Scalar> centerLeftBottom(from[0], from[1], nodeCenter[2]);
        const Vector3<Scalar> centerBottom(nodeCenter[0], from[1], nodeCenter[2]);
        const Vector3<Scalar> centerRightBottom(to[0], from[1], nodeCenter[2]);
        const Vector3<Scalar> centerFrontBottom(nodeCenter[0], from[1], to[2]);

        const Vector3<Scalar> centerBackLeft(from[0], nodeCenter[1], from[2]);
        const Vector3<Scalar> centerBack(nodeCenter[0], nodeCenter[1], from[2]);
        const Vector3<Scalar> centerBackRight(to[0], nodeCenter[1], from[2]);
        const Vector3<Scalar> centerLeft(from[0], nodeCenter[1], nodeCenter[2]);

        const Vector3<Scalar> centerRight(to[0], nodeCenter[1], nodeCenter[2]);
        const Vector3<Scalar> centerFrontLeft(from[0], nodeCenter[1], to[2]);
        const Vector3<Scalar> centerFront(nodeCenter[0], nodeCenter[1], to[2]);
        const Vector3<Scalar> centerFrontRight(to[0], nodeCenter[1], to[2]);

        const Vector3<Scalar> centerBackTop(nodeCenter[0], to[1], from[2]);
        const Vector3<Scalar> centerLeftTop(from[0], to[1], nodeCenter[2]);
        const Vector3<Scalar> centerTop(nodeCenter[0], to[1], nodeCenter[2]);
        const Vector3<Scalar> centerRightTop(to[0], to[1], nodeCenter[2]);
        const Vector3<Scalar> centerFrontTop(nodeCenter[0], to[1], to[2]);

        Vector3<Scalar> positions[19][2] =
        {
            { centerBackBottom, Vector3<Scalar>(0.5f, 0.0f, 0.0f) },
            { centerLeftBottom, Vector3<Scalar>(0.0f, 0.0f, 0.5f) },
            { centerBottom, Vector3<Scalar>(0.5f, 0.0f, 0.5f) },
            { centerRightBottom, Vector3<Scalar>(1.0f, 0.0f, 0.5f) },
            { centerFrontBottom, Vector3<Scalar>(0.5f, 0.0f, 1.0f) },

            { centerBackLeft, Vector3<Scalar>(0.0f, 0.5f, 0.0f) },
            { centerBack, Vector3<Scalar>(0.5f, 0.5f, 0.0f) },
            { centerBackRight, Vector3<Scalar>(1.0f, 0.5f, 0.0f) },
            { centerLeft, Vector3<Scalar>(0.0f, 0.5f, 0.5f) },
            { nodeCenter, Vector3<Scalar>(0.5f, 0.5f, 0.5f) },
            { centerRight, Vector3<Scalar>(1.0f, 0.5f, 0.5f) },
            { centerFrontLeft, Vector3<Scalar>(0.0f, 0.5f, 1.0f) },
            { centerFront, Vector3<Scalar>(0.5f, 0.5f, 1.0f) },
            { centerFrontRight, Vector3<Scalar>(1.0f, 0.5f, 1.0f) },

            { centerBackTop, Vector3<Scalar>(0.5f, 1.0f, 0.0f) },
            { centerLeftTop, Vector3<Scalar>(0.0f, 1.0f, 0.5f) },
            { centerTop, Vector3<Scalar>(0.5f, 1.0f, 0.5f) },
            { centerRightTop, Vector3<Scalar>(1.0f, 1.0f, 0.5f) },
            { centerFrontTop, Vector3<Scalar>(0.5f, 1.0f, 1.0f) }
        };

        Scalar error = 0.0f;
        Vector4<Scalar> value;
        Vector3<Scalar> gradient;

        const VolumeBase& volumeBase = *(super::m_VolumeSource);
        for (onyxU8 i = 0; i < 19; ++i)
        {
            value = volumeBase.GetValueAndGradient(positions[i][0]);
            gradient[0] = value[0];
            gradient[1] = value[1];
            gradient[2] = value[2];
            Scalar interpolated = Interpolate(f000, f001, f010, f011, f100, f101, f110, f111, positions[i][1]);
            Scalar gradientMagnitude = numeric_cast<Scalar>(gradient.Length());
            if (gradientMagnitude < numeric_limits<Scalar>::epsilon())
            {
                gradientMagnitude = 1.0f;
            }
            error += abs(value[3] - interpolated) / gradientMagnitude;
            if (error >= m_MaxGeometricError)
            {
                return true;
            }
        }

        return false;
    }

    bool HasComplexSurface(const Vector4<Scalar> (&cornerValues)[8])
    {
        for (onyxU8 i = 0; i < 7; ++i)
        {
            for (onyxU8 j = i + 1; j < 8; ++j)
            {
                if (cornerValues[i].Dot3D(cornerValues[j]) < m_ComplexSurfaceThreshold)
                {
                    return true;
                }
            }
        }

        return false;
    }

    Scalar Interpolate(Scalar f000, Scalar f001, Scalar f010, Scalar f011,
        Scalar f100, Scalar f101, Scalar f110, Scalar f111, const Vector3<Scalar>& position)
    {
        const Scalar oneMinX = 1.0f - position[0];
        const Scalar oneMinY = 1.0f - position[1];
        const Scalar oneMinZ = 1.0f - position[2];
        const Scalar oneMinXoneMinY = oneMinX * oneMinY;
        const Scalar xOneMinY = position[0] * oneMinY;
        return oneMinZ * (f000* oneMinXoneMinY
            + f100 * xOneMinY
            + f010 * oneMinX * position[1])
            + position[2] * (f001* oneMinXoneMinY
                + f101 * xOneMinY
                + f011 * oneMinX * position[1])
            + position[0] * position[1] * (f110* oneMinZ
                + f111 * position[2]);
    }
private:
    Scalar m_MaxGeometricError;
    Scalar m_ComplexSurfaceThreshold;
};

}