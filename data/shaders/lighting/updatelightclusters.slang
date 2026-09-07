import lighting;
import includes.math.matrix;

struct LightCount {
    uint32_t PointLights;
    uint32_t SpotLights;
};

struct PushConstants {
    float4x4 ViewMatrix;

    Ptr< LightCluster > Cluster;
    Ptr< LightGrid > Grid;

    Ptr< LightCount > GlobalLightCount;
    Ptr< LightIndices > LightIndices;

    Ptr< PointLights > PointLights;
    Ptr< SpotLights > SpotLights;
};

[ push_constant ]
PushConstants Constants;

static const int32_t InvalidLightIndex = -1;

// Shared variables
groupshared uint32_t sharedPointLightIndices[ UpdateClustersThreadsXYZ ];
groupshared uint32_t sharedSpotLights[ UpdateClustersThreadsXYZ ];

float squaredDistanceToAABB( float3 position, uint32_t tileIndex ) {
    float squaredDistance = 0.0;
    AxisAlignedBoundingBox currentCell = Constants.Cluster->Cells[ tileIndex ];
    for( uint32_t i = 0; i < 3; ++i ) {
        float v = position[ i ];
        if( v < currentCell.Min[ i ] ) {
            squaredDistance += ( currentCell.Min[ i ] - v ) * ( currentCell.Min[ i ] - v );
        }
        if( v > currentCell.Max[ i ] ) {
            squaredDistance += ( v - currentCell.Max[ i ] ) * ( v - currentCell.Max[ i ] );
        }
    }

    return squaredDistance;
}

bool isPointLightOverappingAABB( in PointLight light, uint clusterIndex, uint groupIndex ) {
    float radius = light.Radius;
    float4 center = Constants.ViewMatrix * float4( light.Position, 1.0 );
    float squaredDistance = squaredDistanceToAABB( center.xyz, clusterIndex );
    return squaredDistance <= ( radius * radius );
}

bool isSpotLightOverlappingAABB( in SpotLight light, uint clusterIndex ) {
    // inspired by https://bartwronski.com/2017/04/13/cull-that-cone/
    // we construct a bounding sphere for the cluster AABB and check that against the cone
    AxisAlignedBoundingBox currentCell = Constants.Cluster.Cells[ clusterIndex ];

    float3 boundingSphereCenter = ( currentCell.Max.xyz + currentCell.Min.xyz ) * 0.5;
    float3 boundingBoxExtents = ( currentCell.Max.xyz - currentCell.Min.xyz ) * 0.5;
    float boundingSphereRadius = dot( boundingBoxExtents, boundingBoxExtents );

    float4 lightPos = Constants.ViewMatrix * float4( light.Position, 1.0 );
    float3 lightDirection = float3x3( Constants.ViewMatrix ) * light.Direction;

    const float3 V = boundingSphereCenter - lightPos.xyz;
    const float VlenSq = dot( V, V );
    const float V1len = dot( V, lightDirection );
    const float distanceClosestPoint = cos( radians( light.Angle ) ) * sqrt( VlenSq - V1len * V1len ) -
                                       V1len * sin( radians( light.Angle ) );

    const bool angleCull = distanceClosestPoint > boundingSphereRadius;
    const bool frontCull = V1len > boundingSphereRadius + light.Range;
    const bool backCull = V1len < -boundingSphereRadius;

    return !( angleCull || frontCull || backCull );
}

[ shader( "compute" ) ]
[ numthreads( UpdateClustersThreadsX, UpdateClustersThreadsY, UpdateClustersThreadsZ ) ]
void computeMain( uint3 groupId: SV_GroupID, uint32_t groupIndex: SV_GroupIndex ) {
    Constants.GlobalLightCount->PointLights = 0;
    Constants.GlobalLightCount->SpotLights = 0;

    uint32_t threadCount = UpdateClustersThreadsX * UpdateClustersThreadsY * UpdateClustersThreadsZ;

    uint32_t clusterIndex = groupIndex + threadCount * groupId.z;

    uint32_t visiblePointLightCount = 0;
    uint32_t visibleSpotLightCount = 0;
    uint32_t visiblePointLightIndices[ MaxLights ];
    uint32_t visibleSpotLightIndices[ MaxLights ];

    uint32_t lightCount = Constants.PointLights->Count;
    uint32_t numBatches = ( lightCount + ( threadCount - 1 ) ) / threadCount;
    for( uint32_t batchIndex = 0; batchIndex < numBatches; ++batchIndex ) {
        uint32_t pointLightIndex = batchIndex * threadCount + groupIndex;

        sharedPointLightIndices[ groupIndex ] = ( pointLightIndex < lightCount ) ? pointLightIndex : InvalidLightIndex;
        GroupMemoryBarrierWithGroupSync();

        // Iterating within current batchIndex of lights
        for( uint32_t i = 0; i < threadCount; ++i ) {
            uint32_t lightIndex = sharedPointLightIndices[ i ];
            if( lightIndex == InvalidLightIndex ) {
                continue;
            }

            PointLight light = Constants.PointLights->Lights[ lightIndex ];
            if( isPointLightOverappingAABB( light, clusterIndex, groupIndex ) ) {
                visiblePointLightIndices[ visiblePointLightCount ] = batchIndex * threadCount + i;
                visiblePointLightCount += 1;
            }
        }
    }

    //    spot light
    lightCount = Constants.SpotLights.Count;
    numBatches = ( lightCount + ( threadCount - 1 ) ) / threadCount;
    for( uint32_t batchIndex = 0; batchIndex < numBatches; ++batchIndex ) {
        uint32_t spotLightIndex = batchIndex * threadCount + groupIndex;
        sharedPointLightIndices[ groupIndex ] = ( spotLightIndex < lightCount ) ? spotLightIndex : InvalidLightIndex;

        GroupMemoryBarrierWithGroupSync();

        // Iterating within current batchIndex of lights
        for( uint32_t i = 0; i < threadCount; ++i ) {
            uint32_t lightIndex = sharedPointLightIndices[ i ];
            if( lightIndex == InvalidLightIndex ) {
                continue;
            }

            SpotLight light = Constants.SpotLights->Lights[ lightIndex ];
            if( isSpotLightOverlappingAABB( light, clusterIndex ) ) {
                visibleSpotLightIndices[ visibleSpotLightCount ] = batchIndex * threadCount + i;
                ++visibleSpotLightCount;
            }
        }
    }

    // All thread groups have completed bounds checks
    GroupMemoryBarrierWithGroupSync();

    uint32_t pointLightOffset;
    InterlockedAdd( Constants.GlobalLightCount->PointLights, visiblePointLightCount, pointLightOffset );

    uint32_t spotLightOffset;
    InterlockedAdd( Constants.GlobalLightCount->SpotLights, visibleSpotLightCount, spotLightOffset );

    for( uint32_t i = 0; i < visiblePointLightCount; ++i ) {
        Constants.LightIndices->Indices[ pointLightOffset + i ].PointLightIndex = visiblePointLightIndices[ i ];
    }

    for( uint32_t i = 0; i < visibleSpotLightCount; ++i ) {
        Constants.LightIndices->Indices[ spotLightOffset + i ].SpotLightIndex = visibleSpotLightIndices[ i ];
    }

    LightGridCell cell;
    cell.PointLightOffset = pointLightOffset;
    cell.PointLightCount = visiblePointLightCount;
    cell.SpotLightOffset = spotLightOffset;
    cell.SpotLightCount = visibleSpotLightCount;
    Constants.Grid.Cells[ clusterIndex ] = cell;
}
