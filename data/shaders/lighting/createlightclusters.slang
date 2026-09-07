import lighting;

import includes.math.matrix;

struct PushConstants {
    float4x4 InverseProjectionMatrix;

    float2 Viewport;
    float zNear;
    float zFar;

    Ptr< LightCluster > Cluster;
    uint2 ClusterSize;
};

[ push_constant ]
PushConstants Constants;

float4 clipSpaceToViewspace( float4 positionClipSpace ) {
    float4 positionViewSpace = Constants.InverseProjectionMatrix * positionClipSpace;
    positionViewSpace /= positionViewSpace.w;
    return positionViewSpace;
}

float4 screenSpaceToViewspace( float4 positionScreenSpace ) {
    // Convert to NDC
    float2 texCoord = positionScreenSpace.xy / Constants.Viewport.xy;

    // Convert to Clip
    float4 clipSpacePosition = float4( float2( texCoord.x, 1.0 - texCoord.y ) * 2.0 - 1.0f,
                                       positionScreenSpace.z,
                                       positionScreenSpace.w );
    return clipSpaceToViewspace( clipSpacePosition );
}

// Creates a line from the eye/camera to the screenpoint, then finds its intersection
// With a z oriented plane located at the given distance to the origin
float3 lineIntersectionToZPlane( float3 a, float3 b, float zDistance ) {
    // z plane normal
    float3 normal = float3( 0.0, 0.0, 1.0 );

    float3 ab = b - a;

    // Get interesection to plane
    float t = ( zDistance - dot( normal, a ) ) / dot( normal, ab );

    float3 result = a + t * ab;
    return result;
}

[ shader( "compute" ) ]
[ numthreads( 1, 1, 1 ) ]
void computeMain( uint3 groupID: SV_GroupID ) {
    // Camera position in view space
    const float3 cameraPosition = float3( 0.0 );

    // Per Tile
    uint tileIndex = groupID.x + ( groupID.y * ClustersX ) + ( groupID.z * ( ClustersX * ClustersY ) );

    // Calculate Min/Max in screen space
    float4 minPointScreenSpace = float4( groupID.xy * Constants.ClusterSize, 0.0, 1.0 ); // Top Left
    float4 maxPointScreenSpace = float4( float2( groupID.x + 1, groupID.y + 1 ) * Constants.ClusterSize,
                                         0.0,
                                         1.0 ); // Bottom right

    // Convert Min/Max to view space
    float3 minPointViewSpace = screenSpaceToViewspace( minPointScreenSpace ).xyz;
    float3 maxPointViewSpace = screenSpaceToViewspace( maxPointScreenSpace ).xyz;

    // Near and far values of the cluster in view space
    float tileNear = -Constants.zNear * pow( Constants.zFar / Constants.zNear, groupID.z / float( ClustersZ ) );
    float tileFar = -Constants.zNear * pow( Constants.zFar / Constants.zNear, ( groupID.z + 1 ) / float( ClustersZ ) );

    // Finding the 4 intersection points
    float3 minPointNear = lineIntersectionToZPlane( cameraPosition, minPointViewSpace, tileNear );
    float3 minPointFar = lineIntersectionToZPlane( cameraPosition, minPointViewSpace, tileFar );
    float3 maxPointNear = lineIntersectionToZPlane( cameraPosition, maxPointViewSpace, tileNear );
    float3 maxPointFar = lineIntersectionToZPlane( cameraPosition, maxPointViewSpace, tileFar );

    float3 minPointAABB = min( min( minPointNear, minPointFar ), min( maxPointNear, maxPointFar ) );
    float3 maxPointAABB = max( max( minPointNear, minPointFar ), max( maxPointNear, maxPointFar ) );

    Constants.Cluster->Cells[ tileIndex ].Min = float4( minPointAABB, 0.0 );
    Constants.Cluster->Cells[ tileIndex ].Max = float4( maxPointAABB, 0.0 );
}
