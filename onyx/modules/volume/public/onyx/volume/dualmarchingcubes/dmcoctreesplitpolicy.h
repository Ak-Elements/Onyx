#pragma once

#include <onyx/volume/octree/octreesplitpolicy.h>
#include <onyx/volume/chunk/volumechunk.h>
#include <onyx/volume/octree/octreenode.h>

#ifndef USE_ANALYTICAL_NORMAL
#define USE_ANALYTICAL_NORMAL 0
#endif


namespace Onyx::Volume
{
    // Given an edge it gives back the two
    // cell vertices that connect it
    // @ at pos 0 and 1 - the order will always
    // be in the positive direction...
    //
    const onyxU8 EDGE_VERTICES[12][2] = {
        {2, 6}, // edge 0
        {0, 4}, // edge 1
        {0, 2}, // edge 2
        {4, 6}, // edge 3
        {1, 5}, // edge 4
        {3, 7}, // edge 5
        {1, 3}, // edge 6
        {5, 7}, // edge 7
        {0, 1}, // edge 8
        {4, 5}, // edge 9
        {2, 3}, // edge 10
        {6, 7}  // edge 11
    };

template <typename Scalar>
class DMCOctreeSplitPolicy : public OctreeSplitPolicy<Scalar>
{
private:
    using super = OctreeSplitPolicy<Scalar>;
public:
    DMCOctreeSplitPolicy(onyxU8 maxOctreeLevel, Scalar octreeRootSize, Scalar maxGeometricError, Scalar sampleResolution, Scalar complexSurfaceThreshold, const VolumeBase& volumeBase)
        : super(maxOctreeLevel, octreeRootSize, volumeBase)
        , m_MaxGeometricError(maxGeometricError)
        , m_SampleResolution(sampleResolution)
        , m_ComplexSurfaceThreshold(complexSurfaceThreshold)
    {
    }

    void SetUseTriPlanarError(bool val) { m_UseTriplanarError = val; }
    void SetUseEdgeAmbiguity(bool val) { m_UseCMSSplit = val; }

    bool ShouldSplit(VolumeChunkOctree::OctreeNodeT& node, const Vector3<Scalar>& nodeWorldPosition, Scalar halfExtent, onyxU8 nodeLevel) override
    {
        using std::abs;
        using std::numeric_limits;

        const VolumeBase& volumeBase = *(super::m_VolumeSource);

        if ((31 - nodeLevel) > super::m_MaxOctreeLevel)
        {
            return false;
        }

        if (m_UseCMSSplit && halfExtent < m_SampleResolution)
        {
            return false;
        }

        Vector4<Scalar> centerValue = volumeBase.GetValueAndGradient(nodeWorldPosition);

        Vector3<Scalar> halfExtentVec(halfExtent);
        Vector3<Scalar> from = nodeWorldPosition - halfExtentVec;
        Vector3<Scalar> to = nodeWorldPosition + halfExtentVec;

        if (abs(centerValue[3]) > (to - from).Length())
        {
            // set value
            node.GetData()->Gradient = centerValue;
            return false;
        }

		bool shouldSplit = false;
		if (m_UseTriplanarError)
		{
			shouldSplit = CheckTriplanarGeometricError(from, nodeWorldPosition, to);
		}

		if (m_UseCMSSplit)
		{
			shouldSplit = CheckCMSLike(nodeWorldPosition, halfExtent);
		}

		if (shouldSplit)
			return true;

        node.GetData()->Gradient = centerValue;
        return false;
    }

private:
	bool CheckTriplanarGeometricError(const Vector3<Scalar>& from, const Vector3<Scalar>& center, const Vector3<Scalar>& to)
	{
		const VolumeBase& volumeBase = *(super::m_VolumeSource);

		const Vector3<Scalar> corner1(to[0], from[1], from[2]);
		const Vector3<Scalar> corner2(to[0], from[1], to[2]);
		const Vector3<Scalar> corner3(from[0], from[1], to[2]);
		const Vector3<Scalar> corner4(from[0], to[1], from[2]);
		const Vector3<Scalar> corner5(to[0], to[1], from[2]);
		const Vector3<Scalar> corner7(from[0], to[1], to[2]);

		// Error metric of http://www.andrew.cmu.edu/user/jessicaz/publication/meshing/
		onyxF32 f000 = volumeBase.GetValue(from);
		onyxF32 f001 = volumeBase.GetValue(corner3);
		onyxF32 f010 = volumeBase.GetValue(corner4);
		onyxF32 f011 = volumeBase.GetValue(corner7);
		onyxF32 f100 = volumeBase.GetValue(corner1);
		onyxF32 f101 = volumeBase.GetValue(corner2);
		onyxF32 f110 = volumeBase.GetValue(corner5);
		onyxF32 f111 = volumeBase.GetValue(to);

		const Vector3<Scalar> centerBackBottom(center[0], from[1], from[2]);
		const Vector3<Scalar> centerLeftBottom(from[0], from[1], center[2]);
		const Vector3<Scalar> faceBottom(center[0], from[1], center[2]);
		const Vector3<Scalar> centerRightBottom(to[0], from[1], center[2]);
		const Vector3<Scalar> centerFrontBottom(center[0], from[1], to[2]);

		const Vector3<Scalar> centerBackLeft(from[0], center[1], from[2]);
		const Vector3<Scalar> faceBack(center[0], center[1], from[2]);
		const Vector3<Scalar> centerBackRight(to[0], center[1], from[2]);
		const Vector3<Scalar> faceLeft(from[0], center[1], center[2]);

		const Vector3<Scalar> faceRight(to[0], center[1], center[2]);
		const Vector3<Scalar> centerFrontLeft(from[0], center[1], to[2]);
		const Vector3<Scalar> faceFront(center[0], center[1], to[2]);
		const Vector3<Scalar> centerFrontRight(to[0], center[1], to[2]);

		const Vector3<Scalar> centerBackTop(center[0], to[1], from[2]);
		const Vector3<Scalar> centerLeftTop(from[0], to[1], center[2]);
		const Vector3<Scalar> faceTop(center[0], to[1], center[2]);
		const Vector3<Scalar> centerRightTop(to[0], to[1], center[2]);
		const Vector3<Scalar> centerFrontTop(center[0], to[1], to[2]);

		Vector3<Scalar> positions[19][2] =
		{
			{ centerBackBottom, Vector3<Scalar>(0.5f, 0.0f, 0.0f) },
			{ centerLeftBottom, Vector3<Scalar>(0.0f, 0.0f, 0.5f) },
			{ faceBottom, Vector3<Scalar>(0.5f, 0.0f, 0.5f) },
			{ centerRightBottom, Vector3<Scalar>(1.0f, 0.0f, 0.5f) },
			{ centerFrontBottom, Vector3<Scalar>(0.5f, 0.0f, 1.0f) },

			{ centerBackLeft, Vector3<Scalar>(0.0f, 0.5f, 0.0f) },
			{ faceBack, Vector3<Scalar>(0.5f, 0.5f, 0.0f) },
			{ centerBackRight, Vector3<Scalar>(1.0f, 0.5f, 0.0f) },
			{ faceLeft, Vector3<Scalar>(0.0f, 0.5f, 0.5f) },
			{ center, Vector3<Scalar>(0.5f, 0.5f, 0.5f) },
			{ faceRight, Vector3<Scalar>(1.0f, 0.5f, 0.5f) },
			{ centerFrontLeft, Vector3<Scalar>(0.0f, 0.5f, 1.0f) },
			{ faceFront, Vector3<Scalar>(0.5f, 0.5f, 1.0f) },
			{ centerFrontRight, Vector3<Scalar>(1.0f, 0.5f, 1.0f) },

			{ centerBackTop, Vector3<Scalar>(0.5f, 1.0f, 0.0f) },
			{ centerLeftTop, Vector3<Scalar>(0.0f, 1.0f, 0.5f) },
			{ faceTop, Vector3<Scalar>(0.5f, 1.0f, 0.5f) },
			{ centerRightTop, Vector3<Scalar>(1.0f, 1.0f, 0.5f) },
			{ centerFrontTop, Vector3<Scalar>(0.5f, 1.0f, 1.0f) }
		};

		Scalar error = 0.0f;
		Vector4<Scalar> value;
		Vector3<Scalar> gradient;

		for (onyxU8 i = 0; i < 19; ++i)
		{
			value = volumeBase.GetValueAndGradient(positions[i][0]);
			gradient[0] = value[0];
			gradient[1] = value[1];
			gradient[2] = value[2];
			Scalar interpolated = Interpolate(f000, f001, f010, f011, f100, f101, f110, f111, positions[i][1]);
			Scalar gradientMagnitude = numeric_cast<Scalar>(gradient.Length());
			if (gradientMagnitude < 1.0f)
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
	
    bool CheckCMSLike(const Vector3<Scalar>& center, const Scalar halfExtent)
	{
        const Vector3f CORNER_0(-1.0f, -1.0f, -1.0f);
        const Vector3f CORNER_1(1.0f, -1.0f, -1.0f);
        const Vector3f CORNER_2(1.0f, -1.0f, 1.0f);
        const Vector3f CORNER_3(-1.0f, -1.0f, 1.0f);
        const Vector3f CORNER_4(-1.0f, 1.0f, -1.0f);
        const Vector3f CORNER_5(1.0f, 1.0f, -1.0f);
        const Vector3f CORNER_6(1.0f, 1.0f, 1.0f);
        const Vector3f CORNER_7(-1.0f, 1.0f, 1.0f);

        const VolumeBase& volumeBase = *(super::m_VolumeSource);

        const Vector3<Scalar> corners[8] =
        {
            center + (CORNER_0 * halfExtent),
            center + (CORNER_1 * halfExtent), // corner1
            center + (CORNER_2 * halfExtent), // corner2
            center + (CORNER_3 * halfExtent), // corner3
            center + (CORNER_4 * halfExtent), // corner4
            center + (CORNER_5 * halfExtent), // corner5
            center + (CORNER_6 * halfExtent),
            center + (CORNER_7 * halfExtent)  // corner7

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

        if (CheckEdgeAmbiguity(corners))
        {
            return true;
        }

        if (HasComplexSurface(values))
        {
            return true;
        }

        return false;
	}

    bool CheckEdgeAmbiguity(const Vector3<Scalar>(&corners)[8])
    {
        const VolumeBase& volumeBase = *(super::m_VolumeSource);

        // Getting the index of the c000 point of the current cell
        //const Index3D *indPtr = c->getPointInds();

        bool hasFoundCrossing = false;
        // Loop through all the edges of the cell
        for (int i = 0; i < 12; ++i)
        {
            // Getting the start and end cell points of this edge
            const Vector3f& edgeStartCorner = corners[EDGE_VERTICES[i][0]];
            const Vector3f& edgeEndCorner = corners[EDGE_VERTICES[i][1]];

            const Vector3f direction = (edgeEndCorner - edgeStartCorner).Normalized() * m_SampleResolution;

            // Resetting the crossing point of this edge to zero
            hasFoundCrossing = false;

            // Get the edge direction from the static table
            //uint8_t edgeDirection = EDGE_DIRECTION[i];

            onyxF32 previousSample = volumeBase.GetValue(edgeStartCorner);
            Vector3f currentSamplePosition = edgeStartCorner;

            auto compare = [](const Vector3f& a, const Vector3f& b)
                {
                    return a[0] < b[0] && a[1] < b[1] && a[2] < b[2];
                };

            while (compare(currentSamplePosition, edgeEndCorner))
            {
                onyxF32 currentSample = volumeBase.GetValue(currentSamplePosition);
                if (previousSample * currentSample < 0.f)
                {
                    if (hasFoundCrossing) // if a second crossing is found this node has an edge ambiguity
                    {
                        return true;
                    }

                    hasFoundCrossing = true;
                }

                previousSample = currentSample;
                currentSamplePosition += direction;
            }
        }

        return false;
    }

    bool HasComplexSurface(const Vector4<Scalar>(&cornerValues)[8])
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
    const Scalar m_MaxGeometricError = 0;
	bool m_UseTriplanarError = true;
	bool m_UseCMSSplit = false;

    const Scalar m_SampleResolution = 1.0f;
    const Scalar m_ComplexSurfaceThreshold = 1.0f;
};

}