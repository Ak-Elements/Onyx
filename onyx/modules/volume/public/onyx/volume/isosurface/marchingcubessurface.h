﻿#pragma once

#include <onyx/volume/isosurface/isosurface.h>
#include <onyx/volume/isosurface/marchingcubestable.h>
#include <onyx/volume/source/volumebase.h>
#include <onyx/volume/mesh/meshbuilder.h>

namespace Onyx::Volume
{
    class VolumeBase;

    template <typename T>
    class MarchingCubesSurface : public IsoSurface<T>
    {
    private:
        using super = IsoSurface<T>;
    private:
        MeshBuilder* m_MeshBuilder = nullptr;

    public:
        explicit  MarchingCubesSurface(const VolumeBase* volume)
            : super(volume)
        {
        }

        void SetMeshBuilder(MeshBuilder& mb) { m_MeshBuilder = &mb; }

        void AddTriangles(const Vector3<T> corners[], const Vector4<T> volumeValues[], const Vector3<T> valuesAnalyticalNormals[]) const override
        {
            unsigned char cubeIndex = 0;
            Vector4<T> values[8];

            // Find out the case.
            for (size_t i = 0; i < 8; ++i)
            {
                if (volumeValues)
                {
                    values[i] = volumeValues[i];
                }
                else
                {
                    values[i] = super::m_Volume->GetValueAndGradient(corners[i]); // todo change gradient to normal
                }
                onyxF32 gradient = values[i][3];
                if (gradient >= super::ISO_LEVEL)
                {
                    cubeIndex |= 1 << i;
                }
            }

            const int edge = MarchingCubes::Edges[cubeIndex];

            // Nothing intersects.
            if (!edge)
            {
                return;
            }

            // Find the intersection vertices.
            Vector3<T> intersectionPoints[12];
            Vector3<T> intersectionNormals[12];

            // todo: remove
            Vector3<T> intersectionAnalyticalNormals[12];
            auto analyticalNormalsFunc = [&corners, &values, &intersectionAnalyticalNormals, &valuesAnalyticalNormals, this](int cornerIndex0, int cornerIndex1, int valuesIndex0, int valuesIndex1, int outIndex)
            {
                super::CalculateSurfacePositionAndNormal(corners[cornerIndex0], corners[cornerIndex1], Vector4f(valuesAnalyticalNormals[valuesIndex0], values[valuesIndex0][3]), Vector4f(valuesAnalyticalNormals[valuesIndex1], values[valuesIndex1][3]), intersectionAnalyticalNormals[outIndex]);
            };
            // end remove

            if (edge & 1)
            {
                intersectionPoints[0] = super::CalculateSurfacePositionAndNormal(corners[0], corners[1], values[0], values[1], intersectionNormals[0]);
                analyticalNormalsFunc(0, 1, 0, 1, 0);
            }
            if (edge & 2)
            {
                intersectionPoints[1] = super::CalculateSurfacePositionAndNormal(corners[1], corners[2], values[1], values[2], intersectionNormals[1]);
                analyticalNormalsFunc(1, 2, 1, 2, 1);
            }
            if (edge & 4)
            {
                intersectionPoints[2] = super::CalculateSurfacePositionAndNormal(corners[2], corners[3], values[2], values[3], intersectionNormals[2]);
                analyticalNormalsFunc(2, 3, 2, 3, 2);
            }
            if (edge & 8)
            {
                intersectionPoints[3] = super::CalculateSurfacePositionAndNormal(corners[3], corners[0], values[3], values[0], intersectionNormals[3]);
                analyticalNormalsFunc(3, 0, 3, 0, 3);
            }
            if (edge & 16)
            {
                intersectionPoints[4] = super::CalculateSurfacePositionAndNormal(corners[4], corners[5], values[4], values[5], intersectionNormals[4]);
                analyticalNormalsFunc(4, 5, 4, 5, 4);
            }
            if (edge & 32)
            {
                intersectionPoints[5] = super::CalculateSurfacePositionAndNormal(corners[5], corners[6], values[5], values[6], intersectionNormals[5]);
                analyticalNormalsFunc(5, 6, 5, 6, 5);
            }
            if (edge & 64)
            {
                intersectionPoints[6] = super::CalculateSurfacePositionAndNormal(corners[6], corners[7], values[6], values[7], intersectionNormals[6]);
                analyticalNormalsFunc(6, 7, 6, 7, 6);
            }
            if (edge & 128)
            {
                intersectionPoints[7] = super::CalculateSurfacePositionAndNormal(corners[7], corners[4], values[7], values[4], intersectionNormals[7]);
                analyticalNormalsFunc(7, 4, 7, 4, 7);
            }
            if (edge & 256)
            {
                intersectionPoints[8] = super::CalculateSurfacePositionAndNormal(corners[0], corners[4], values[0], values[4], intersectionNormals[8]);
                analyticalNormalsFunc(0, 4, 0, 4, 8);
            }
            if (edge & 512)
            {
                intersectionPoints[9] = super::CalculateSurfacePositionAndNormal(corners[1], corners[5], values[1], values[5], intersectionNormals[9]);
                analyticalNormalsFunc(1, 5, 1, 5, 9);
            }
            if (edge & 1024)
            {
                intersectionPoints[10] = super::CalculateSurfacePositionAndNormal(corners[2], corners[6], values[2], values[6], intersectionNormals[10]);
                analyticalNormalsFunc(2, 6, 2, 6, 10);
            }
            if (edge & 2048)
            {
                intersectionPoints[11] = super::CalculateSurfacePositionAndNormal(corners[3], corners[7], values[3], values[7], intersectionNormals[11]);
                analyticalNormalsFunc(3, 7, 3, 7, 11);
            }

            // Create the triangles according to the table.
            for (unsigned char i = 0; MarchingCubes::Triangles[cubeIndex][i] != -1; i += 3)
            {
                m_MeshBuilder->AddTriangle(
                    intersectionPoints[MarchingCubes::Triangles[cubeIndex][i]], intersectionNormals[MarchingCubes::Triangles[cubeIndex][i]], intersectionAnalyticalNormals[MarchingCubes::Triangles[cubeIndex][i]],
                    intersectionPoints[MarchingCubes::Triangles[cubeIndex][i + 1]], intersectionNormals[MarchingCubes::Triangles[cubeIndex][i + 1]], intersectionAnalyticalNormals[MarchingCubes::Triangles[cubeIndex][i + 1]],
                    intersectionPoints[MarchingCubes::Triangles[cubeIndex][i + 2]], intersectionNormals[MarchingCubes::Triangles[cubeIndex][i + 2]], intersectionAnalyticalNormals[MarchingCubes::Triangles[cubeIndex][i + 2]]);
            }
        }
    };
}
