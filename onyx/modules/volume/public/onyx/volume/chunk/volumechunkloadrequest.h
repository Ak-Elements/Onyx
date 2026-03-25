#pragma once

#include <onyx/onyx_types.h>

#include <onyx/thread/async/asynctask.h>
#include <onyx/volume/chunk/volumechunk.h>
#include <onyx/volume/isosurface/isosurface.h>
#include <onyx/volume/mesh/meshbuilder.h>

namespace onyx::volume {
class VolumeChunkLoader;

struct VolumeChunckLoadRequestData {
    VolumeChunckLoadRequestData( IsoSurfaceMethod isoSurfaceMethod,
                                 const Vector3f32& position,
                                 uint8_t maxOctreeLevel,
                                 float32 octreeRootSize,
                                 float32 maxGeometricError,
                                 float32 sampleResolution,
                                 float32 complexSurfaceThreshold,
                                 const VolumeBase& volumeBase )
        : m_Position( position )
        , m_Size( octreeRootSize )
        , m_MaxOctreeLevel( maxOctreeLevel )
        , m_MaxGeometricError( maxGeometricError )
        , m_SampleResolution( sampleResolution )
        , m_ComplexSurfaceThreshold( complexSurfaceThreshold )
        , m_IsoSurfaceMethod( isoSurfaceMethod )
        , m_VolumeSource( &volumeBase )

    {}

    VolumeChunckLoadRequestData( const VolumeChunckLoadRequestData& rhs )
        : m_Position( rhs.m_Position )
        , m_Size( rhs.m_Size )
        , m_MaxOctreeLevel( rhs.m_MaxOctreeLevel )
        , m_MaxGeometricError( rhs.m_MaxGeometricError )
        , m_SampleResolution( rhs.m_SampleResolution )
        , m_ComplexSurfaceThreshold( rhs.m_ComplexSurfaceThreshold )
        , m_MaxDistanceSkirts( rhs.m_MaxDistanceSkirts )
        , m_IsoSurfaceMethod( rhs.m_IsoSurfaceMethod )
        , m_VolumeSource( rhs.m_VolumeSource ) {}

  public:
    Vector3f32 m_Position{ 0.0f };
    float32 m_Size = 0.0f;

    uint8_t m_MaxOctreeLevel = std::numeric_limits< uint8_t >::max();
    float32 m_MaxGeometricError = 1.0f;
    float32 m_SampleResolution = 1.0f;
    float32 m_ComplexSurfaceThreshold = 0.85f;
    float32 m_MaxDistanceSkirts = 100;

    IsoSurfaceMethod m_IsoSurfaceMethod;

    // TODO: should not be part of the load request but the task
    MeshBuilder m_MeshBuilder;

    const VolumeBase* m_VolumeSource = nullptr;
};

class VolumeChunkLoadRequest {
  public:
    VolumeChunkLoadRequest( const VolumeChunckLoadRequestData& loadRequestData,
                            InplaceFunction< void( const VolumeChunckLoadRequestData& ), 64 >&& finishedCallback )
        : m_LoadRequestData( loadRequestData )
        , m_FinishedCallback( std::move( finishedCallback ) ) {}

    ~VolumeChunkLoadRequest();

    void Load();
    void Cancel( bool waitForCancel = false );

    bool IsFinished() { return m_LoadingTaskFuture.isPending() == false; }

  private:
    void LoadChunk();

    void GenerateOctree( VolumeChunk::VolumeChunkOctree& octree );
    void GenerateOctreeChildren( VolumeChunk::VolumeChunkOctree::OctreeNodeT* node, uint8_t octreeLevel );
    VolumeOctreeNodeMetaData GetNodeMetaData( const Vector3f32& nodeLocalPosition, float32 nodeHalfExtents );

    void GenerateDualGridNode();

  private:
    VolumeChunckLoadRequestData m_LoadRequestData;
    threading::Future< void > m_LoadingTaskFuture;

    InplaceFunction< void( const VolumeChunckLoadRequestData& ), 64 > m_FinishedCallback;
};

} // namespace onyx::volume
