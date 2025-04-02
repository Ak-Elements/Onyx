#include <onyx/volume/chunk/volumechunkloadrequest.h>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/threadpool/threadpool.h>
#include <onyx/volume/source/volumebase.h>
#include <onyx/volume/dualmarchingcubes/dmcoctreesplitpolicy.h>
#include <onyx/volume/cubicalmarchingsquares/cmsoctreesplitpolicy.h>
#include <onyx/volume/isosurface/marchingcubessurface.h>
#include <onyx/volume/isosurface/marchingsquaressurface.h>
#include <onyx/volume/chunk/volumechunk.h>

namespace Onyx::Volume
{
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

        //if (m_LoadingTaskFuture.IsCancelled())
        //    return;

        // octree
        GenerateOctree(volumeOctree);

        //if (m_LoadingTaskFuture.IsCancelled())
        //    return;

        // dualgrid
        MarchingCubesSurface<onyxF32> marchingCubesSurface(m_LoadRequestData.m_VolumeSource);
        marchingCubesSurface.SetMeshBuilder(m_LoadRequestData.m_MeshBuilder);

        MarchingSquaresSurface<onyxF32> marchingSquaresSurface(m_LoadRequestData.m_VolumeSource, m_LoadRequestData.m_MeshBuilder, m_LoadRequestData.m_MaxDistanceSkirts);

        VolumeChunkDualgrid& dualgrid = volumeDualgrid;
        dualgrid.GetDualCells().clear();

        dualgrid.SetIsoSurface(&marchingCubesSurface);
        dualgrid.SetMarchingSquaresIsoSurface(&marchingSquaresSurface);

        dualgrid.GenerateDualgrid(volumeOctree.GetRootNode(), m_LoadRequestData.m_Position, volumeBase);
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
                Vector4f& centerValue = node.GetData()->Gradient;
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

    VolumeOctreeNodeMetaData VolumeChunkLoadRequest::GetNodeMetaData(const Vector3f& nodeLocalPosition, onyxF32 nodeHalfExtents)
    {
        VolumeOctreeNodeMetaData metaData = VolumeOctreeNodeMetaData::None;
        const onyxF32 rootNodeHalfExtent = m_LoadRequestData.m_Size * 0.5f;
        const Vector3f from = nodeLocalPosition - Vector3f { nodeHalfExtents };
        const Vector3f to = nodeLocalPosition + Vector3f { nodeHalfExtents };

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
