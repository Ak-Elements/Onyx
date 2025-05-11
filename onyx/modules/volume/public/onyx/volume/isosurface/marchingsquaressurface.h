#pragma once

#include <onyx/volume/isosurface/isosurface.h>

#include <onyx/volume/isosurface/marchingsquarestable.h>

#include <onyx/volume/source/volumebase.h>
#include <onyx/volume/mesh/meshbuilder.h>

namespace Onyx::Volume
{
    template <typename T>
    class MarchingSquaresSurface : public IsoSurface<T>
    {
    private:
        using super = IsoSurface<T>;
    public:
        explicit MarchingSquaresSurface(const VolumeBase* volume, MeshBuilder& meshBuilder, const T maxDistance)
            : super(volume)
            , m_MeshBuilder(&meshBuilder)
            , m_MaxDistance(maxDistance)
        {
        }

        void AddMarchingSquaresTriangles(const Vector3<T> corners[], const Vector4<T> volumeValues[], const onyxU8 indices[])
        {
            unsigned char squareIndex = 0;
            Vector4<T> values[4];

            // Find out the case.
            for (size_t i = 0; i < 4; ++i)
            {
                if (volumeValues)
                {
                    values[i] = volumeValues[indices[i]];
                }
                else
                {
                    values[i] = super::m_Volume->GetValueAndGradient(corners[indices[i]]);
                }

                if (values[i][3] >= super::ISO_LEVEL)
                {
                    squareIndex |= 1 << i;
                }
            }

            // Don't generate triangles if we are completely inside and farPlane enough away from the surface
            if (squareIndex == 15 && values[0][3] >= m_MaxDistance && values[1][3] >= m_MaxDistance && values[2][3] >= m_MaxDistance && values[3][3] >= m_MaxDistance)
            {
                return;
            }

            const int edge = MarchingSquares::Edges[squareIndex];

            // Find the intersection vertices.
            Vector3<T> intersectionPoints[8];
            Vector3<T> intersectionNormals[8];
            intersectionPoints[0] = corners[indices[0]];
            intersectionPoints[2] = corners[indices[1]];
            intersectionPoints[4] = corners[indices[2]];
            intersectionPoints[6] = corners[indices[3]];

            Vector4f innerVal = super::m_Volume->GetValueAndGradient(intersectionPoints[0]);
            intersectionNormals[0][0] = innerVal[0];
            intersectionNormals[0][1] = innerVal[1];
            intersectionNormals[0][2] = innerVal[2];
            intersectionNormals[0].Normalize();
            intersectionNormals[0] *= innerVal[3] + 1.0f;

            innerVal = super::m_Volume->GetValueAndGradient(intersectionPoints[2]);
            intersectionNormals[2][0] = innerVal[0];
            intersectionNormals[2][1] = innerVal[1];
            intersectionNormals[2][2] = innerVal[2];
            intersectionNormals[2].Normalize();
            intersectionNormals[2] *= innerVal[3] + 1.0f;

            innerVal = super::m_Volume->GetValueAndGradient(intersectionPoints[4]);
            intersectionNormals[4][0] = innerVal[0];
            intersectionNormals[4][1] = innerVal[1];
            intersectionNormals[4][2] = innerVal[2];
            intersectionNormals[4].Normalize();
            intersectionNormals[4] *= innerVal[3] + 1.0f;

            innerVal = super::m_Volume->GetValueAndGradient(intersectionPoints[6]);
            intersectionNormals[6][0] = innerVal[0];
            intersectionNormals[6][1] = innerVal[1];
            intersectionNormals[6][2] = innerVal[2];
            intersectionNormals[6].Normalize();
            intersectionNormals[6] *= innerVal[3] + 1.0f;

            if (edge & 1)
            {
                intersectionPoints[1] = super::CalculateSurfacePositionAndNormal(corners[indices[0]], corners[indices[1]], values[0], values[1], intersectionNormals[1]);
            }
            if (edge & 2)
            {
                intersectionPoints[3] = super::CalculateSurfacePositionAndNormal(corners[indices[1]], corners[indices[2]], values[1], values[2], intersectionNormals[3]);
            }
            if (edge & 4)
            {
                intersectionPoints[5] = super::CalculateSurfacePositionAndNormal(corners[indices[2]], corners[indices[3]], values[2], values[3], intersectionNormals[5]);
            }
            if (edge & 8)
            {
                intersectionPoints[7] = super::CalculateSurfacePositionAndNormal(corners[indices[3]], corners[indices[0]], values[3], values[0], intersectionNormals[7]);
            }

            // Create the triangles according to the table.
            for (unsigned char i = 0; MarchingSquares::Triangles[squareIndex][i] != -1; i += 3)
            {
                m_MeshBuilder->AddTriangle(
                    intersectionPoints[MarchingSquares::Triangles[squareIndex][i]], intersectionNormals[MarchingSquares::Triangles[squareIndex][i]], intersectionNormals[MarchingSquares::Triangles[squareIndex][i]],
                    intersectionPoints[MarchingSquares::Triangles[squareIndex][i + 1]], intersectionNormals[MarchingSquares::Triangles[squareIndex][i + 1]], intersectionNormals[MarchingSquares::Triangles[squareIndex][i + 1]],
                    intersectionPoints[MarchingSquares::Triangles[squareIndex][i + 2]], intersectionNormals[MarchingSquares::Triangles[squareIndex][i + 2]], intersectionNormals[MarchingSquares::Triangles[squareIndex][i + 2]]);
            }
        }

        void AddTriangles(const Vector3<T> corners[], const Vector4<T> volumeValues[], const Vector3<T> valuesAnalyticalNormals[]) const override
        {
            ONYX_UNUSED(corners);
            ONYX_UNUSED(volumeValues);
            ONYX_UNUSED(valuesAnalyticalNormals);
        }

    private: 
        MeshBuilder* m_MeshBuilder;

        const T m_MaxDistance{};
    };
}