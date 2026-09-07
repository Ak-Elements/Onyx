import includes.volume.volumesource;
import includes.math.functions;

struct CsgEllipsoid {
    float3 Position;
    float3 Radii;

    __init( float3 position, float3 radii ) {
        Position = position;
        Radii = radii;
    }

    __init( in float[ ] packedData ) { unpack( packedData ); }

    __init( in Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) { unpack( sourcesData, sourceIndex ); }

    float4 getValueAndGradient( float3 worldPosition ) {
        float3 difference = worldPosition - Position;
        float3 radiiSquared = Radii * Radii;
        float k0 = length( difference * safeReciprocal( Radii ) );

        float3 gradient = difference * safeReciprocal( radiiSquared );
        float k1 = safeReciprocal( length( gradient ) );
        return float4( ( gradient * k1 ), k0 * ( k0 - 1.0 ) * k1 );
    }

    void pack( inout float[ ] data ) {
        data[ 0 ] = Position.x;
        data[ 1 ] = Position.y;
        data[ 2 ] = Position.z;
        data[ 3 ] = Radii.x;
        data[ 4 ] = Radii.y;
        data[ 5 ] = Radii.z;
    }

    void pack( Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) {
        sourcesData->Data[ sourceIndex + 0 ] = Position.x;
        sourcesData->Data[ sourceIndex + 1 ] = Position.y;
        sourcesData->Data[ sourceIndex + 2 ] = Position.z;
        sourcesData->Data[ sourceIndex + 3 ] = Radii.x;
        sourcesData->Data[ sourceIndex + 4 ] = Radii.y;
        sourcesData->Data[ sourceIndex + 5 ] = Radii.z;
    }

    [ mutating ]
    void unpack( in Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) {
        Position.x = sourcesData->Data[ sourceIndex ];
        Position.y = sourcesData->Data[ sourceIndex + 1 ];
        Position.z = sourcesData->Data[ sourceIndex + 2 ];
        Radii.x = sourcesData->Data[ sourceIndex + 3 ];
        Radii.y = sourcesData->Data[ sourceIndex + 4 ];
        Radii.z = sourcesData->Data[ sourceIndex + 5 ];
    }

    [ mutating ]
    void unpack( in float[ ] data ) {
        Position.x = data[ 0 ];
        Position.y = data[ 1 ];
        Position.z = data[ 2 ];
        Radii.x = data[ 3 ];
        Radii.y = data[ 4 ];
        Radii.z = data[ 5 ];
    }
};

