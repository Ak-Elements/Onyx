import includes.bindless;
import includes.math.functions;

struct BiplanarResult {
    int2 MajorAxis;
    int2 MedianAxis;

    int2 WeightAxis;
    int2 MajorSigns;
    int2 MedianSigns;
}

// Reoriented Normal Mapping blend (Ben Golus / Colin Barré-Brisebois technique).
// n1 is treated as the "base" normal, n2 as the detail normal being applied on top of it.
float3 blendReorientedNormalMapping( float3 n1, float3 n2 ) {
    n1.z += 1.0f;
    n2.xy = -n2.xy;
    return n1 * dot( n1, n2 ) * safeReciprocal( n1.z ) - n2;
}

BiplanarResult getBiplanarAxis( float3 normal ) {
    int3 signs = sign( normal );
    float3 absNormal = abs( normal );
    int3 majorAxis = ( ( absNormal.x > absNormal.y ) && ( absNormal.x > absNormal.z ) ) ? int3( 0, 2, 1 )
                     : ( absNormal.y > absNormal.z )                                    ? int3( 1, 0, 2 )
                                                                                        : int3( 2, 0, 1 );

    int3 minorAxis = ( ( absNormal.x < absNormal.y ) && ( absNormal.x < absNormal.z ) ) ? int3( 0, 2, 1 )
                     : ( absNormal.y < absNormal.z )                                    ? int3( 1, 0, 2 )
                                                                                        : int3( 2, 0, 1 );

    int3 medianAxis = int3( 3 ) - minorAxis - majorAxis;

    BiplanarResult result = BiplanarResult( majorAxis.yz, medianAxis.yz, int2( majorAxis.x, medianAxis.x ) );
    if( majorAxis.x == 0 ) {
        result.MajorSigns = int2( -signs.x, -1 );
    } else if( majorAxis.x == 1 ) {
        result.MajorSigns = int2( 1 );
    } else {
        result.MajorSigns = int2( signs.z, -1 );
    }

    if( medianAxis.x == 0 ) {
        result.MedianAxis = int2( -signs.x, -1 );
    } else if( medianAxis.x == 1 ) {
        result.MedianAxis = int2( 1 );
    } else {
        result.MedianAxis = int2( signs.z, -1 );
    }
    return result;
}

float4 sampleBiplanar( uint32_t textureIndex, float3 position, float3 normal ) {
    normal = abs( normal );
    BiplanarResult biplanar = getBiplanarAxis( normal );
    float2 uv0 = float2( position[ biplanar.MajorAxis.x ], position[ biplanar.MajorAxis.y ] );

    float2 uv1 = float2( position[ biplanar.MedianAxis.x ], position[ biplanar.MedianAxis.y ] );

    float4 majorAxisSample = linearSample( textureIndex, uv0, 0 );
    float4 medianAxisSample = linearSample( textureIndex, uv1, 0 );

    float2 weights = float2( normal[ biplanar.WeightAxis.x ], normal[ biplanar.WeightAxis.y ] );
    weights = clamp( ( weights - 0.5773f ) / ( 1.0f - 0.5773f ), 0.0f, 1.0f );

    return ( majorAxisSample * weights.x + medianAxisSample * weights.y ) * safeReciprocal( weights.x + weights.y );
}

float4 sampleBiplanar( uint32_t textureIndex,
                       BiplanarResult biplanar,
                       float3 position,
                       float3 normal,
                       float3 ddx,
                       float3 ddy ) {
    float3 absNormal = abs( normal );
    float3 signs = sign( normal );

    float2 uv0 = float2( position[ biplanar.MajorAxis.x ], position[ biplanar.MajorAxis.y ] ) * biplanar.MajorSigns;
    float2 ddx0 = float2( ddx[ biplanar.MajorAxis.x ], ddx[ biplanar.MajorAxis.y ] ) * biplanar.MajorSigns;
    float2 ddy0 = float2( ddy[ biplanar.MajorAxis.x ], ddy[ biplanar.MajorAxis.y ] ) * biplanar.MajorSigns;

    float2 uv1 = float2( position[ biplanar.MedianAxis.x ], position[ biplanar.MedianAxis.y ] ) * biplanar.MedianSigns;
    float2 ddx1 = float2( ddx[ biplanar.MedianAxis.x ], ddx[ biplanar.MedianAxis.y ] ) * biplanar.MedianSigns;
    float2 ddy1 = float2( ddy[ biplanar.MedianAxis.x ], ddy[ biplanar.MedianAxis.y ] ) * biplanar.MedianSigns;

    float4 majorAxisSample = linearSampleGrad( textureIndex, uv0, ddx0, ddy0 );
    float4 medianAxisSample = linearSampleGrad( textureIndex, uv1, ddx1, ddy1 );

    float2 weights = float2( absNormal[ biplanar.WeightAxis.x ], absNormal[ biplanar.WeightAxis.y ] );
    weights = clamp( ( weights - 0.5773f ) / ( 1.0f - 0.5773f ), 0.0f, 1.0f );

    return ( majorAxisSample * weights.x + medianAxisSample * weights.y ) * safeReciprocal( weights.x + weights.y );
}

float3 sampleNormalBiplanar( uint32_t textureIndex,
                             BiplanarResult biplanar,
                             float3 position,
                             float3 geometricNormal, // SIGNED world-space normal
                             float3 ddx,
                             float3 ddy ) {
    geometricNormal = normalize( geometricNormal );
    float3 absNormal = abs( geometricNormal );

    float sign0 = sign( geometricNormal[ biplanar.WeightAxis.x ] );
    float sign1 = sign( geometricNormal[ biplanar.WeightAxis.y ] );

    float2 uv0 = float2( position[ biplanar.MajorAxis.x ], position[ biplanar.MajorAxis.y ] ) * biplanar.MajorSigns;
    float2 ddx0 = float2( ddx[ biplanar.MajorAxis.x ], ddx[ biplanar.MajorAxis.y ] ) * biplanar.MajorSigns;
    float2 ddy0 = float2( ddy[ biplanar.MajorAxis.x ], ddy[ biplanar.MajorAxis.y ] ) * biplanar.MajorSigns;

    float3 tnormal0 = linearSampleGrad( textureIndex, uv0, ddx0, ddy0 ).xyz * 2.0f - 1.0f;

    float3 refNormal0 = float3( geometricNormal[ biplanar.MajorAxis.x ] * sign0,
                                geometricNormal[ biplanar.MajorAxis.y ] * sign0,
                                absNormal[ biplanar.WeightAxis.x ] );

    float3 blended0 = blendReorientedNormalMapping( refNormal0, tnormal0 );

    float3 worldNormal0 = float3( 0.0f, 0.0f, 0.0f );
    worldNormal0[ biplanar.MajorAxis.x ] = blended0.x * sign0;
    worldNormal0[ biplanar.MajorAxis.y ] = blended0.y * sign0;
    worldNormal0[ biplanar.WeightAxis.x ] = blended0.z * sign0;

    float2 uv1 = float2( position[ biplanar.MedianAxis.x ], position[ biplanar.MedianAxis.y ] ) * biplanar.MedianSigns;
    float2 ddx1 = float2( ddx[ biplanar.MedianAxis.x ], ddx[ biplanar.MedianAxis.y ] ) * biplanar.MedianSigns;
    float2 ddy1 = float2( ddy[ biplanar.MedianAxis.x ], ddy[ biplanar.MedianAxis.y ] ) * biplanar.MedianSigns;

    float3 tnormal1 = linearSampleGrad( textureIndex, uv1, ddx1, ddy1 ).xyz * 2.0f - 1.0f;

    // Construct base normal relative to the projection sign direction
    float3 refNormal1 = float3( geometricNormal[ biplanar.MedianAxis.x ] * sign1,
                                geometricNormal[ biplanar.MedianAxis.y ] * sign1,
                                absNormal[ biplanar.WeightAxis.y ] );

    float3 blended1 = blendReorientedNormalMapping( refNormal1, tnormal1 );

    float3 worldNormal1 = float3( 0.0f, 0.0f, 0.0f );
    worldNormal1[ biplanar.MedianAxis.x ] = blended1.x * sign1;
    worldNormal1[ biplanar.MedianAxis.y ] = blended1.y * sign1;
    worldNormal1[ biplanar.WeightAxis.y ] = blended1.z * sign1;

    float2 weights = float2( absNormal[ biplanar.WeightAxis.x ], absNormal[ biplanar.WeightAxis.y ] );
    weights = clamp( ( weights - 0.5773f ) / ( 1.0f - 0.5773f ), 0.0f, 1.0f );
    weights *= safeReciprocal( weights.x + weights.y );

    return normalize( worldNormal0 * weights.x + worldNormal1 * weights.y );
}

float4 sampleBiplanar( uint32_t textureIndex, float3 position, float3 normal, float3 ddx, float3 ddy ) {
    BiplanarResult biplanar = getBiplanarAxis( normal );
    return sampleBiplanar( textureIndex, biplanar, position, normal, ddx, ddy );
}
