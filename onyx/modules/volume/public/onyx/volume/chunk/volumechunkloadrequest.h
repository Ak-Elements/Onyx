#pragma once

#include <onyx/onyx_types.h>
#include <onyx/volume/octree/octree.h>
#include <onyx/volume/octree/octreesplitpolicy.h>

#include <onyx/volume/mesh/meshbuilder.h>
#include <onyx/thread/async/asynctask.h>
#include <onyx/volume/onyx_volume_pch.h>
#include <onyx/volume/isosurface/isosurface.h>

namespace Onyx::Volume
{
    class VolumeChunkLoader;

struct VolumeChunckLoadRequestData
{
    VolumeChunckLoadRequestData(IsoSurfaceMethod isoSurfaceMethod, const Vector3f& position, onyxU8 maxOctreeLevel, onyxF32 octreeRootSize, onyxF32 maxGeometricError, onyxF32 sampleResolution, onyxF32 complexSurfaceThreshold, const VolumeBase& volumeBase)
        : m_Position(position)
        , m_Size(octreeRootSize)
        , m_MaxOctreeLevel(maxOctreeLevel)
        , m_MaxGeometricError(maxGeometricError)
        , m_SampleResolution(sampleResolution)
        , m_ComplexSurfaceThreshold(complexSurfaceThreshold)
        , m_IsoSurfaceMethod(isoSurfaceMethod)
        , m_VolumeSource(&volumeBase)

    {
    }

    VolumeChunckLoadRequestData(const VolumeChunckLoadRequestData& rhs)
        : m_Position(rhs.m_Position)
        , m_Size(rhs.m_Size)
        , m_MaxOctreeLevel(rhs.m_MaxOctreeLevel)
        , m_MaxGeometricError(rhs.m_MaxGeometricError)
        , m_SampleResolution(rhs.m_SampleResolution)
        , m_ComplexSurfaceThreshold(rhs.m_ComplexSurfaceThreshold)
        , m_MaxDistanceSkirts(rhs.m_MaxDistanceSkirts)
        , m_IsoSurfaceMethod(rhs.m_IsoSurfaceMethod)
        , m_VolumeSource(rhs.m_VolumeSource)
    {
    }

public:
    Vector3f m_Position{ 0.0f };
    onyxF32 m_Size = 0.0f;

    onyxU8 m_MaxOctreeLevel = onyxMax_U8;
    onyxF32 m_MaxGeometricError = 1.0f;
    onyxF32 m_SampleResolution = 1.0f;
    onyxF32 m_ComplexSurfaceThreshold = 0.85f;
    onyxF32 m_MaxDistanceSkirts = 100;

    IsoSurfaceMethod m_IsoSurfaceMethod;

    //TODO: should not be part of the load request but the task
    MeshBuilder m_MeshBuilder;

    const VolumeBase* m_VolumeSource = nullptr;
};

class VolumeChunkLoadRequest
{
public:
    VolumeChunkLoadRequest(const VolumeChunckLoadRequestData& loadRequestData, InplaceFunction<void(const VolumeChunckLoadRequestData&), 64>&& finishedCallback)
        : m_LoadRequestData(loadRequestData)
        , m_FinishedCallback(std::move(finishedCallback))
    {
    }

    ~VolumeChunkLoadRequest();

    void Load();
    void Cancel(bool waitForCancel = false);

    bool IsFinished() { return m_LoadingTaskFuture.IsPending() == false; }

private:
    void LoadChunk();

    void GenerateOctree(VolumeChunkOctree& octree);
    void GenerateOctreeChildren(VolumeChunkOctree::OctreeNodeT* node, onyxU8 octreeLevel);
    VolumeOctreeNodeMetaData GetNodeMetaData(const Vector3f& nodeLocalPosition, onyxF32 nodeHalfExtents);

    void GenerateDualGridNode();

private:
    VolumeChunckLoadRequestData m_LoadRequestData;
    Threading::Future<void> m_LoadingTaskFuture;

    InplaceFunction<void(const VolumeChunckLoadRequestData&), 64> m_FinishedCallback;
};

}
