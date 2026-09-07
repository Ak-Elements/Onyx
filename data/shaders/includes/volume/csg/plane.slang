
struct CsgPlane {
    float3 Normal;
    float Distance;

    __init( float3 normal, float distance ) {
        Normal = normal;
        Distance = distance;
    }

    __init( in float[ ] packedData ) { unpack( packedData ); }

    float4 getValueAndGradient( float3 worldPosition ) {
        return float4( Normal.x, Normal.y, Normal.z, dot( Normal, worldPosition ) - Distance );
    }

    void pack( inout float[ ] data ) {
        data[ 0 ] = Normal.x;
        data[ 1 ] = Normal.y;
        data[ 2 ] = Normal.z;
        data[ 3 ] = Distance;
    }

    [ mutating ]
    void unpack( in float[ ] data ) {
        Normal.x = data[ 0 ];
        Normal.y = data[ 1 ];
        Normal.z = data[ 2 ];
        Distance = data[ 3 ];
    }
};

