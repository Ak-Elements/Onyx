#include <onyx/volume/chunk/volumechunkloadrequest.h>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/threadpool/threadpool.h>
#include <onyx/volume/source/volumebase.h>
#include <onyx/volume/dualmarchingcubes/dmcoctreesplitpolicy.h>
#include <onyx/volume/cubicalmarchingsquares/cmsoctreesplitpolicy.h>
#include <onyx/volume/isosurface/marchingcubessurface.h>
#include <onyx/volume/isosurface/marchingsquaressurface.h>
#include <onyx/volume/chunk/volumechunk.h>
#include <onyx/volume/isosurface/marchingsquaressurface_cms.h>

namespace Onyx::Volume
{
    namespace
    {

        //      -----
        //      | 0 |             - z
        //  -----------------     ^ 
        //  | 4 | 2 | 5 | 3 |     |
        //  -----------------     ---> x
        //      | 1 |
        //      -----

        //      2---6
        //      | 0 |              - z
        //  2---0---4---6---2     ^ 
        //  | 4 | 2 | 5 | 3 |     |
        //  3---1---5---7---3     ---> x
        //      | 1 |
        //      3---7


        //      v0 +--e0--+ v2      - z 
        //         +      +         ^  
        //         e2     e3        | 
        //         +      +         ---> x 
        //      v1 +--e1--+ v3      

        const onyxU8 FACE_VERTICES[6][4] =
        {
            {6,4,2,0},
            {5,7,1,3},
            {4,5,0,1},
            {2,3,6,7},
            {0,1,2,3},
            {6,7,4,5}

            //{0, 2, 4, 6}, // back face
            //{1, 3, 5, 7}, // front face
            //{0, 1, 4, 5}, // bottom
            //{2, 3, 6, 7}, // top
            //{0, 1, 2, 3}, // left
            //{4, 5, 6, 7}, // right
        };

        //
//      Vertex and Edge Index Map
//
//            2-------0------6
//           /.             /|
//          10.           11 |
//         /  2           /  3
//        /   .          /   |     ^ Y
//       3-------5------7    |     |
//       |    0 . . 1 . |. . 4     --> X
//       |   .          |   /
//       6  8           7  9
//       | .            | /
//       |.             |/
//       1-------4------5 
//                        

        struct Component
        {
            DynamicArray<Vertex> Vertices;
        };

        void ExtractMesh(const Vector3f32& octreeRootPosition, const OctreeNode<VolumeData>& octreeNode, const VolumeBase& csgSource, CubicalMarchingSquares::MarchingSquares<onyxF32> cubicalMarchingSquares, MeshBuilder& meshBuilder)
        {
            const VolumeData& octreeData = octreeNode.GetData();
            /*const Vector3f CORNER_0(-1.0f, -1.0f, -1.0f);
            const Vector3f CORNER_1(1.0f, -1.0f, -1.0f);
            const Vector3f CORNER_2(1.0f, -1.0f, 1.0f);
            const Vector3f CORNER_3(-1.0f, -1.0f, 1.0f);
            const Vector3f CORNER_4(-1.0f, 1.0f, -1.0f);
            const Vector3f CORNER_5(1.0f, 1.0f, -1.0f);
            const Vector3f CORNER_6(1.0f, 1.0f, 1.0f);
            const Vector3f CORNER_7(-1.0f, 1.0f, 1.0f);*/

            ONYX_UNUSED(octreeRootPosition);
            ONYX_UNUSED(octreeData);

            const Vector3f32 nodePosition;// octreeRootPosition + octreeData->Position;
            const float halfExtents = 4.0f; //octreeData->HalfExtent;
            Vector3f32 worldPositionCorner0 = nodePosition + Vector3f32(-1, -1, -1) * halfExtents; // 0
            Vector3f32 worldPositionCorner1 = nodePosition + Vector3f32(-1, -1, 1) * halfExtents; // 1
            Vector3f32 worldPositionCorner2 = nodePosition + Vector3f32(-1, 1, -1) * halfExtents; // 2
            Vector3f32 worldPositionCorner3 = nodePosition + Vector3f32(-1, 1, 1) * halfExtents; // 3
            Vector3f32 worldPositionCorner4 = nodePosition + Vector3f32(1, -1, -1) * halfExtents; // 4
            Vector3f32 worldPositionCorner5 = nodePosition + Vector3f32(1, -1, 1) * halfExtents; // 5
            Vector3f32 worldPositionCorner6 = nodePosition + Vector3f32(1, 1, -1) * halfExtents; // 6
            Vector3f32 worldPositionCorner7 = nodePosition + Vector3f32(1, 1, 1) * halfExtents; // 7

            Onyx::InplaceArray<Onyx::Vector3f32, 8> corners       
            {
                worldPositionCorner0,
                worldPositionCorner1,
                worldPositionCorner2,
                worldPositionCorner3,
                worldPositionCorner4,
                worldPositionCorner5,
                worldPositionCorner6,
                worldPositionCorner7
            };

            Vector4f32 gradientCorner0 = csgSource.GetValueAndGradient(worldPositionCorner0);
            Vector4f32 gradientCorner1 = csgSource.GetValueAndGradient(worldPositionCorner1);
            Vector4f32 gradientCorner2 = csgSource.GetValueAndGradient(worldPositionCorner2);
            Vector4f32 gradientCorner3 = csgSource.GetValueAndGradient(worldPositionCorner3);
            Vector4f32 gradientCorner4 = csgSource.GetValueAndGradient(worldPositionCorner4);
            Vector4f32 gradientCorner5 = csgSource.GetValueAndGradient(worldPositionCorner5);
            Vector4f32 gradientCorner6 = csgSource.GetValueAndGradient(worldPositionCorner6);
            Vector4f32 gradientCorner7 = csgSource.GetValueAndGradient(worldPositionCorner7);

            Onyx::InplaceArray<Onyx::Vector4f32, 8> hermiteDataSamples
            {
                gradientCorner0,
                gradientCorner1,
                gradientCorner2,
                gradientCorner3,
                gradientCorner4,
                gradientCorner5,
                gradientCorner6,
                gradientCorner7,
            };

            // run cms

            using LineSegment = Onyx::Volume::CubicalMarchingSquares::Internal::LineSegment<Onyx::onyxF32>;
            Onyx::InplaceArray<LineSegment, 24> outLineSegements;

            Onyx::InplaceArray<Onyx::Vector3f32, 4> faceCorners;
            Onyx::InplaceArray<Onyx::Vector4f32, 4> hermiteData;

            InplaceArray<onyxU8, 6> faceCases;
            for (Onyx::onyxU8 faceIndex = 0; faceIndex < 6; ++faceIndex)
            {
                faceCorners[0] = corners[FACE_VERTICES[faceIndex][0]];
                faceCorners[1] = corners[FACE_VERTICES[faceIndex][1]];
                faceCorners[2] = corners[FACE_VERTICES[faceIndex][2]];
                faceCorners[3] = corners[FACE_VERTICES[faceIndex][3]];

                hermiteData[0] = hermiteDataSamples[FACE_VERTICES[faceIndex][0]];
                hermiteData[1] = hermiteDataSamples[FACE_VERTICES[faceIndex][1]];
                hermiteData[2] = hermiteDataSamples[FACE_VERTICES[faceIndex][2]];
                hermiteData[3] = hermiteDataSamples[FACE_VERTICES[faceIndex][3]];

                faceCases.Add(cubicalMarchingSquares.GenerateLines(faceIndex, faceCorners, hermiteData, outLineSegements));
            }

            // useless copy
            DynamicArray<LineSegment> segments;
            for (const LineSegment& lineSegment : outLineSegements)
            {
                segments.push_back(lineSegment);
            }

            // trace segements
            DynamicArray<Component> components;
            onyxU8 edgeIndex = 0;

            while (segments.empty() == false)
            {
                Component& lineStrip = components.emplace_back();
                const LineSegment& segment = segments.back();

                edgeIndex = segment.GetToEdgeIndex();

                for (const auto& linePoint : segment.LinePoints)
                {
                    lineStrip.Vertices.push_back(linePoint);
                }

                segments.pop_back();

                bool hasAdded = true;
                while (hasAdded)
                {
                    hasAdded = false;
                    for (onyxS32 i = static_cast<onyxS32>(segments.size()) - 1; i >= 0; --i)
                    {
                        const LineSegment& lineSegment = segments[i];

                        if (lineSegment.GetFromEdgeIndex() == edgeIndex)
                        {
                            for (const auto& linePoint : lineSegment.LinePoints)
                            {
                                lineStrip.Vertices.push_back(linePoint);
                            }

                            hasAdded = true;
                            edgeIndex = lineSegment.GetToEdgeIndex();
                            segments.erase(segments.begin() + i);
                        }
                        else if (lineSegment.GetToEdgeIndex() == edgeIndex)
                        {
                            for (onyxS32 j = static_cast<onyxS32>(lineSegment.LinePoints.size()); j >= 0; --j)
                            {
                                lineStrip.Vertices.push_back(lineSegment.LinePoints[static_cast<onyxU8>(j)]);
                            }

                            hasAdded = true;
                            edgeIndex = lineSegment.GetFromEdgeIndex();
                            segments.erase(segments.begin() + i);
                        }
                    }
                }

                while (lineStrip.Vertices.front() == lineStrip.Vertices.back())
                {

                    lineStrip.Vertices.erase(lineStrip.Vertices.begin());

                    if (lineStrip.Vertices.empty())
                        break;
                }

                if (lineStrip.Vertices.empty())
                {
                    break;
                }

                //ONYX_ASSERT(lineStrip.Vertices.size() > 2);

            }


            for (Component& component : components)
            {
                if (component.Vertices.empty())
                    continue;

                if (component.Vertices.size() == 3)
                {
                    const Vertex& v0 = component.Vertices[0];
                    const Vertex& v1 = component.Vertices[2];
                    const Vertex& v2 = component.Vertices[1];

                    meshBuilder.AddTriangle(v0.Position,
                        v0.Normal,
                        v1.Position,
                        v1.Normal,
                        v2.Position,
                        v2.Normal); // remove
                }
                else
                {
                    Onyx::onyxU32 vertexCount = static_cast<Onyx::onyxU32>(component.Vertices.size());

                    Vector3f32 triangleFanCenter;
                    Vector3f32 triangleFanCenterNormal;
                    for (const Vertex& vertex : component.Vertices)
                    {
                        triangleFanCenter += vertex.Position;
                        triangleFanCenterNormal += vertex.Normal;
                    }

                    triangleFanCenter /= static_cast<onyxF32>(vertexCount);
                    triangleFanCenterNormal /= static_cast<onyxF32>(vertexCount);

                    onyxU32 lastVertexIndex = (vertexCount - 1);
                    for (onyxU32 i = 0; i < vertexCount; ++i)
                    {
                        if (i == lastVertexIndex)
                        {
                            meshBuilder.AddVertexAndNormal(component.Vertices[0].Position, component.Vertices[0].Normal);
                            meshBuilder.AddVertexAndNormal(component.Vertices[i].Position, component.Vertices[i].Normal);
                            meshBuilder.AddVertexAndNormal(triangleFanCenter, triangleFanCenterNormal);
                        }
                        else
                        {
                            meshBuilder.AddVertexAndNormal(component.Vertices[i + 1].Position, component.Vertices[i + 1].Normal);
                            meshBuilder.AddVertexAndNormal(component.Vertices[i].Position, component.Vertices[i].Normal);
                            meshBuilder.AddVertexAndNormal(triangleFanCenter, triangleFanCenterNormal);
                        }
                    }
                }
            }
        }
    }

    VolumeChunkLoadRequest::~VolumeChunkLoadRequest()
    {
        Cancel(true);
    }

    void VolumeChunkLoadRequest::Load()
    {
        using namespace Threading;
        AsyncTask<void()> loadingTask([this]() { LoadChunk(); });
        m_LoadingTaskFuture = loadingTask.GetFuture();
        m_LoadingTaskFuture.Then([this]() { if (m_FinishedCallback) m_FinishedCallback(m_LoadRequestData); });

        DefaultThreadPool.Post(std::move(loadingTask));
    }

    void VolumeChunkLoadRequest::Cancel(bool waitForCancel/* = false */)
    {
        if (m_LoadingTaskFuture)
            m_LoadingTaskFuture.Cancel(waitForCancel);
    }

    void VolumeChunkLoadRequest::LoadChunk()
    {
        VolumeChunkOctree volumeOctree;
        VolumeChunkDualgrid volumeDualgrid;

        const VolumeBase& volumeBase = *m_LoadRequestData.m_VolumeSource;

        if (m_LoadRequestData.m_IsoSurfaceMethod == IsoSurfaceMethod::CMS)
        {
            GenerateOctree(volumeOctree);

            Volume::CubicalMarchingSquares::MarchingSquares cubicalMarchingSquares(1.0f);

            for (auto leafIt = volumeOctree.leaf_begin(); leafIt != volumeOctree.leaf_end(); ++leafIt)
            {
                const OctreeNode<VolumeData>* node = leafIt.GetCurrentOctreeNode();
                ExtractMesh(m_LoadRequestData.m_Position, *node, volumeBase, cubicalMarchingSquares, m_LoadRequestData.m_MeshBuilder);
            }
        }
        else
        {
            GenerateOctree(volumeOctree);

            MarchingCubesSurface<onyxF32> marchingCubesSurface(m_LoadRequestData.m_VolumeSource);
            marchingCubesSurface.SetMeshBuilder(m_LoadRequestData.m_MeshBuilder);

            MarchingSquaresSurface<onyxF32> marchingSquaresSurface(m_LoadRequestData.m_VolumeSource, m_LoadRequestData.m_MeshBuilder, m_LoadRequestData.m_MaxDistanceSkirts);

            VolumeChunkDualgrid& dualgrid = volumeDualgrid;
            dualgrid.GetDualCells().clear();

            dualgrid.SetIsoSurface(&marchingCubesSurface);
            dualgrid.SetMarchingSquaresIsoSurface(&marchingSquaresSurface);

            dualgrid.GenerateDualgrid(volumeOctree.GetRootNode(), m_LoadRequestData.m_Position, volumeBase);
            
        }
    }

    void VolumeChunkLoadRequest::GenerateOctree(VolumeChunkOctree& octree)
    {
        UniquePtr<OctreeSplitPolicy<onyxF32>> splitPolicy = nullptr;
        if (m_LoadRequestData.m_IsoSurfaceMethod == IsoSurfaceMethod::DMC)
        {
            auto policy = MakeUnique<DMCOctreeSplitPolicy<onyxF32>>(m_LoadRequestData.m_MaxOctreeLevel, m_LoadRequestData.m_Size, m_LoadRequestData.m_MaxGeometricError, m_LoadRequestData.m_SampleResolution, m_LoadRequestData.m_ComplexSurfaceThreshold, *m_LoadRequestData.m_VolumeSource);
            policy->SetUseTriPlanarError(true);
            policy->SetUseEdgeAmbiguity(false);

            splitPolicy = std::move(policy);
        }
        else if (m_LoadRequestData.m_IsoSurfaceMethod == IsoSurfaceMethod::DMC_WITH_CMS_ERROR_METRIC)
        {
            auto policy = MakeUnique<DMCOctreeSplitPolicy<onyxF32>>(m_LoadRequestData.m_MaxOctreeLevel, m_LoadRequestData.m_Size, m_LoadRequestData.m_MaxGeometricError, m_LoadRequestData.m_SampleResolution, m_LoadRequestData.m_ComplexSurfaceThreshold, *m_LoadRequestData.m_VolumeSource);
            policy->SetUseTriPlanarError(false);
            policy->SetUseEdgeAmbiguity(true);

            splitPolicy = std::move(policy);
        }
        else if (m_LoadRequestData.m_IsoSurfaceMethod == IsoSurfaceMethod::CMS)
        {
            splitPolicy = MakeUnique<CMSOctreeSplitPolicy<onyxF32>>(m_LoadRequestData.m_MaxOctreeLevel, m_LoadRequestData.m_Size, m_LoadRequestData.m_SampleResolution, m_LoadRequestData.m_ComplexSurfaceThreshold, *m_LoadRequestData.m_VolumeSource);
        }


        onyxF32 rootNodeSize = m_LoadRequestData.m_Size;
        Vector3<onyxF32> rootNodePosition = m_LoadRequestData.m_Position;

        for (auto it = octree.leaf_begin(); it != octree.leaf_end(); ++it)
        {
            //if (m_LoadingTaskFuture.IsCancelled())
            //    return;

            VolumeChunkOctree::OctreeNodeT& node = *it.GetCurrentOctreeNode();
            VolumeChunkOctree::OctreeKeyT key = it.GetCurrentOctreeKey();
            onyxU8 depth = it.GetCurrentOctreeDepth();

            onyxU8 nodeLevel = (VolumeChunkOctree::OctreeKeyT::MaxDepth - 1) - depth;
            onyxF32 cellSize = rootNodeSize;
            if (nodeLevel > 0)
                cellSize /= (1 << nodeLevel);

            Vector3<onyxF32> nodeLocalPosition = key.GetNodeRealPosition(rootNodeSize, depth);
            Vector3<onyxF32> nodeWorldPosition = nodeLocalPosition + rootNodePosition;

            UniquePtr<VolumeDataContainer>& nodeData = node.GetData();
            nodeData = MakeUnique<VolumeDataContainer>();

            nodeData->Position = nodeLocalPosition;
            nodeData->HalfExtent = cellSize / 2.0f;
            nodeData->MetaData = GetNodeMetaData(nodeData->Position, nodeData->HalfExtent);

            const VolumeBase& volumeBase = *m_LoadRequestData.m_VolumeSource;

            if (splitPolicy->ShouldSplit(node, nodeWorldPosition, nodeData->HalfExtent, depth))
            {
                node.Subdivide();
            }
            else
            {
                Vector4f32& centerValue = node.GetData()->Gradient;
                if (centerValue.IsZero())
                {
                    centerValue = volumeBase.GetValueAndGradient(node.GetData()->Position);

#if USE_ANALYTICAL_NORMAL
                    const SimplexNoiseSource& noiseSource = static_cast<const SimplexNoiseSource&>(volumeBase);
                    node.GetData()->AnalyticalNormal = noiseSource.GetAnalyticalNormal(node.GetData()->Position);
#endif
                }
            }
        }
    }

    VolumeOctreeNodeMetaData VolumeChunkLoadRequest::GetNodeMetaData(const Vector3f32& nodeLocalPosition, onyxF32 nodeHalfExtents)
    {
        VolumeOctreeNodeMetaData metaData = VolumeOctreeNodeMetaData::None;
        const onyxF32 rootNodeHalfExtent = m_LoadRequestData.m_Size * 0.5f;
        const Vector3f32 from = nodeLocalPosition - Vector3f32 { nodeHalfExtents };
        const Vector3f32 to = nodeLocalPosition + Vector3f32 { nodeHalfExtents };

        if (IsEqual(from[0], -rootNodeHalfExtent))
        {
            metaData |= VolumeOctreeNodeMetaData::BorderLeft;
        }
        if (IsEqual(to[0], rootNodeHalfExtent))
        {
            metaData |= VolumeOctreeNodeMetaData::BorderRight;
        }

        if (IsEqual(from[1], -rootNodeHalfExtent))
        {
            metaData |= VolumeOctreeNodeMetaData::BorderBottom;
        }
        if (IsEqual(to[1], rootNodeHalfExtent))
        {
            metaData |= VolumeOctreeNodeMetaData::BorderTop;
        }

        if (IsEqual(from[2], -rootNodeHalfExtent))
        {
            metaData |= VolumeOctreeNodeMetaData::BorderBack;
        }
        if (IsEqual(to[2], rootNodeHalfExtent))
        {
            metaData |= VolumeOctreeNodeMetaData::BorderFront;
        }

        return metaData;
    }
}
