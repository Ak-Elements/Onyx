implementing lighting;

import includes.depth;
import includes.viewconstants;

struct LightGridCell {
    uint PointLightOffset;
    uint PointLightCount;
    uint SpotLightOffset;
    uint SpotLightCount;
};

public struct LightGrid {
    LightGridCell Cells[ ];
}

uint32_t getClusterIndex( in LightEnvironment environment, in ViewConstants constants, float4 fragCoord ) {
    float linearDepth = linearDepth( fragCoord.z, constants );
    uint32_t zTile = uint32_t(
        max( log2( linearDepth ) * environment.LightClusterScale + environment.LightClusterBias, 0.0f ) );
    uint3 clusterIndex3d = uint3( uint2( fragCoord.xy / environment.LightClusterSize ), zTile );
    return clusterIndex3d.x + ( environment.LightClusterGridSize.x * clusterIndex3d.y ) +
           ( ( environment.LightClusterGridSize.x * environment.LightClusterGridSize.y ) * clusterIndex3d.z );
}
