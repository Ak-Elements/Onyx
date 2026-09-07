implementing lighting;

import includes.depth;
import includes.viewconstants;

public struct LightGridCell {
    public uint32_t PointLightOffset;
    public uint32_t PointLightCount;
    public uint32_t SpotLightOffset;
    public uint32_t SpotLightCount;
};

public struct LightGrid {
    public LightGridCell Cells[ ];
}

uint32_t getClusterIndex( in LightEnvironment environment, in ViewConstants constants, float4 fragCoord ) {
    // float linearDepth = linearDepth( fragCoord.z, constants.Near, constants.Far );
    float linearDepth = fragCoord.z;
    uint32_t zTile = uint32_t(
        max( log2( linearDepth ) * environment.LightClusterScale + environment.LightClusterBias, 0.0f ) );
    uint3 clusterIndex3d = uint3( uint2( fragCoord.xy / environment.LightClusterSize ), zTile );
    return clusterIndex3d.x + ( ClustersX * clusterIndex3d.y ) + ( ( ClustersX * ClustersY ) * clusterIndex3d.z );
}
