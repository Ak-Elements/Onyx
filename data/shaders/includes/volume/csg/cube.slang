import includes.volume.volumesource;

struct CsgCube {
    float3 Position;
    float3 HalfExtents;

    __init( float3 position, float3 halfExtents ) {
        Position = position;
        HalfExtents = halfExtents;
    }

    __init( in float[ ] packedData ) { unpack( packedData ); }

    __init( in Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) { unpack( sourcesData, sourceIndex ); }

    float4 getValueAndGradient( float3 worldPosition ) {
        float3 p = worldPosition - Position;
        float3 d = abs( p ) - HalfExtents;

        // Signed distance to the box
        float dist = length( max( d, float3( 0.0 ) ) ) + min( max( d.x, max( d.y, d.z ) ), 0.0 );

        // Exact per-face normal: pick the axis that's most "outside"
        // (or least "inside" if p is within the box), signed by
        // which side of the box p is on along that axis.
        float3 s = sign( p );
        float3 normal;

        if( d.x > d.y && d.x > d.z ) {
            normal = float3( s.x, 0.0, 0.0 );
        } else if( d.y > d.z ) {
            normal = float3( 0.0, s.y, 0.0 );
        } else {
            normal = float3( 0.0, 0.0, s.z );
        }

        return float4( normal, dist );
    }

    void pack( Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) {
        sourcesData->Data[ sourceIndex ] = Position.x;
        sourcesData->Data[ sourceIndex + 1 ] = Position.y;
        sourcesData->Data[ sourceIndex + 2 ] = Position.z;
        sourcesData->Data[ sourceIndex + 3 ] = HalfExtents.x;
        sourcesData->Data[ sourceIndex + 4 ] = HalfExtents.y;
        sourcesData->Data[ sourceIndex + 5 ] = HalfExtents.z;
    }

    void pack( inout float[ ] data ) {
        data[ 0 ] = Position.x;
        data[ 1 ] = Position.y;
        data[ 2 ] = Position.z;
        data[ 3 ] = HalfExtents.x;
        data[ 4 ] = HalfExtents.y;
        data[ 5 ] = HalfExtents.z;
    }

    [ mutating ]
    void unpack( in Ptr< VolumeSourcesData > sourcesData, uint32_t sourceIndex ) {
        Position.x = sourcesData->Data[ sourceIndex ];
        Position.y = sourcesData->Data[ sourceIndex + 1 ];
        Position.z = sourcesData->Data[ sourceIndex + 2 ];
        HalfExtents.x = sourcesData->Data[ sourceIndex + 3 ];
        HalfExtents.y = sourcesData->Data[ sourceIndex + 4 ];
        HalfExtents.z = sourcesData->Data[ sourceIndex + 5 ];
    }

    [ mutating ]
    void unpack( in float[ ] data ) {
        Position.x = data[ 0 ];
        Position.y = data[ 1 ];
        Position.z = data[ 2 ];
        HalfExtents.x = data[ 3 ];
        HalfExtents.y = data[ 4 ];
        HalfExtents.z = data[ 5 ];
    }

    float getClosetDistanceToCube( float3 worldPosition, float3 cubeCenter, float3 halfExtents ) {
        float x = max( worldPosition.x - cubeCenter.x - halfExtents.x, cubeCenter.x - worldPosition.x - halfExtents.x );
        float y = max( worldPosition.y - cubeCenter.y - halfExtents.y, cubeCenter.y - worldPosition.y - halfExtents.y );
        float z = max( worldPosition.z - cubeCenter.z - halfExtents.z, cubeCenter.z - worldPosition.z - halfExtents.z );

        float d = max( x, max( y, z ) );
        return d;
    }
};

