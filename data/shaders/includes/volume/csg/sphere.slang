import includes.volume.volumesource;
import includes.math.constants;
import includes.math.functions;

struct CsgSphere {
    float3 Position;
    float Radius;

    __init( float3 position, float radius ) {
        Position = position;
        Radius = radius;
    }

    __init( in float[ ] packedData ) { unpack( packedData ); }

    __init( in Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) { unpack( sourcesData, sourceIndex ); }

    float4 getValueAndGradient( float3 worldPosition ) {
        const float Epsilon = 0.00001;

        float3 gradient = worldPosition - Position;

        float distance = length( gradient );
        gradient = gradient * ( 1.0 * safeReciprocal( distance + Epsilon ) );

        return float4( gradient, distance - Radius );
    }

    void pack( inout float[ ] data ) {
        data[ 0 ] = Position.x;
        data[ 1 ] = Position.y;
        data[ 2 ] = Position.z;
        data[ 3 ] = Radius;
    }

    void pack( Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) {
        sourcesData->Data[ sourceIndex ] = Position.x;
        sourcesData->Data[ sourceIndex + 1 ] = Position.y;
        sourcesData->Data[ sourceIndex + 2 ] = Position.z;
        sourcesData->Data[ sourceIndex + 3 ] = Radius;
    }

    [ mutating ]
    void unpack( in Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) {
        Position.x = sourcesData->Data[ sourceIndex ];
        Position.y = sourcesData->Data[ sourceIndex + 1 ];
        Position.z = sourcesData->Data[ sourceIndex + 2 ];
        Radius = sourcesData->Data[ sourceIndex + 3 ];
    }

    [ mutating ]
    void unpack( in float[ ] data ) {
        Position.x = data[ 0 ];
        Position.y = data[ 1 ];
        Position.z = data[ 2 ];
        Radius = data[ 3 ];
    }
};
