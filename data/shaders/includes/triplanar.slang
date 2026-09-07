
import includes.bindless;
import includes.math.functions;

// Reoriented Normal Mapping blend (Ben Golus / Colin Barré-Brisebois technique).
// n1 is treated as the "base" normal, n2 as the detail normal being applied on top of it.
float3 blendReorientedNormalMapping( float3 n1, float3 n2 ) {
    n1.z += 1.0f;
    n2.xy = -n2.xy;
    return n1 * dot( n1, n2 ) * safeReciprocal( n1.z ) - n2;
}

float4 sampleTriplanar( uint32_t textureIndex, float3 position, float3 normal, float sharpness = 1.0f ) {
    float2 uvX = position.zy;
    float2 uvY = position.xz;
    float2 uvZ = position.xy;

    float3 axisSign = select( normal < 0.0f, -1.0f, 1.0f );

    float3 triBlend = saturate( pow( normal, sharpness ) );
    triBlend /= max( dot( triBlend, float3( 1.0f, 1.0f, 1.0f ) ), 0.0001f );
    uvX.x *= -axisSign.x;
    uvY.x *= -axisSign.y;
    uvZ.x *= axisSign.z;

    float4 colorX = linearSample( textureIndex, uvX, 0 );
    float4 colorY = linearSample( textureIndex, uvY, 0 );
    float4 colorZ = linearSample( textureIndex, uvZ, 0 );
    return colorX * triBlend.x + colorY * triBlend.y + colorZ * triBlend.z;
}

float4 sampleTriplanar( uint32_t textureIndex,
                        float3 position,
                        float3 normal,
                        float3 ddx,
                        float3 ddy,
                        float sharpness = 1.0f ) {
    float2 uvX = position.zy;
    float2 xDdx = ddx.zy;
    float2 xDdy = ddy.zy;

    float2 uvY = position.xz;
    float2 yDdx = ddx.xz;
    float2 yDdy = ddy.xz;

    float2 uvZ = position.xy;
    float2 zDdx = ddx.xy;
    float2 zDdy = ddy.xy;

    float3 axisSign = select( normal < 0.0f, -1.0f, 1.0f );

    float3 triBlend = saturate( pow( normal, sharpness ) );
    triBlend /= max( dot( triBlend, float3( 1.0f, 1.0f, 1.0f ) ), 0.0001f );
    uvX.x *= -axisSign.x;
    xDdx.x *= -axisSign.x;
    xDdy.x *= -axisSign.x;

    uvY.x *= -axisSign.y;
    yDdx.x *= -axisSign.y;
    yDdy.x *= -axisSign.y;

    uvZ.x *= axisSign.z;
    zDdx.x *= axisSign.z;
    zDdy.x *= axisSign.z;

    float4 colorX = linearSampleGrad( textureIndex, uvX, xDdx, xDdy );
    float4 colorY = linearSampleGrad( textureIndex, uvY, yDdx, yDdy );
    float4 colorZ = linearSampleGrad( textureIndex, uvZ, zDdx, zDdy );
    return colorX * triBlend.x + colorY * triBlend.y + colorZ * triBlend.z;

    // float3 absNormal = abs( normal );
    // float3 weights = pow( absNormal, sharpness );
    // float weightSum = weights.x + weights.y + weights.z;
    // weights /= weightSum;
    //
    // float3 signs = sign( normal );
    //
    // // UV coordinates per axis
    // float2 xUv = position.zy * float2( -signs.x, -1.0f );
    // float2 yUv = position.xz * float2( 1.0f, 1.0f );
    // float2 zUv = position.xy * float2( signs.z, -1.0f );
    //
    // // Projected screen derivatives (ddx) for each plane
    // float2 xDdx = ddx.zy * float2( -signs.x, -1.0f );
    // float2 yDdx = ddx.xz * float2( 1.0f, 1.0f );
    // float2 zDdx = ddx.xy * float2( signs.z, -1.0f );
    //
    // // Projected screen derivatives (ddy) for each plane
    // float2 xDdy = ddy.zy * float2( -signs.x, -1.0f );
    // float2 yDdy = ddy.xz * float2( 1.0f, 1.0f );
    // float2 zDdy = ddy.xy * float2( signs.z, -1.0f );
    //
    // // Sample using explicit gradients to prevent mipmap seam artifacts
    // float3 sampleX = linearSampleGrad( textureIndex, xUv, xDdx, xDdy ).xyz;
    // float3 sampleY = linearSampleGrad( textureIndex, yUv, yDdx, yDdy ).xyz;
    // float3 sampleZ = linearSampleGrad( textureIndex, zUv, zDdx, zDdy ).xyz;
    //
    // return float4( sampleY * weights.y + sampleZ * weights.z + sampleX * weights.x, 1.0f );
}

float3 sampleNormalTriplanar( uint32_t textureIndex,
                              float3 position,
                              float3 geometricNormal, // SIGNED world-space normal
                              float3 ddx,
                              float3 ddy,
                              float sharpness = 1.0f ) {
    float2 uvX = position.zy;
    float2 xDdx = ddx.zy;
    float2 xDdy = ddy.zy;

    float2 uvY = position.xz;
    float2 yDdx = ddx.xz;
    float2 yDdy = ddy.xz;

    float2 uvZ = position.xy;
    float2 zDdx = ddx.xy;
    float2 zDdy = ddy.xy;

    float3 axisSign = select( geometricNormal < 0.0f, -1.0f, 1.0f );

    float3 triBlend = saturate( pow( abs( geometricNormal ), sharpness ) );
    triBlend /= max( dot( triBlend, float3( 1.0f, 1.0f, 1.0f ) ), 0.0001f );
    uvX.x *= axisSign.x;
    xDdx.x *= axisSign.x;
    xDdy.x *= axisSign.x;

    uvY.x *= -axisSign.y;
    yDdx.x *= -axisSign.y;
    yDdy.x *= -axisSign.y;

    uvZ.x *= axisSign.z;
    zDdx.x *= axisSign.z;
    zDdy.x *= axisSign.z;

    float3 tnormalX = linearSampleGrad( textureIndex, uvX, xDdx, xDdy ).xyz * 2.0f - 1.0f;
    float3 tnormalY = linearSampleGrad( textureIndex, uvY, yDdx, yDdy ).xyz * 2.0f - 1.0f;
    float3 tnormalZ = linearSampleGrad( textureIndex, uvZ, zDdx, zDdy ).xyz * 2.0f - 1.0f;

    tnormalX.x *= axisSign.x;
    tnormalY.x *= -1.0f;
    tnormalY.y *= -axisSign.y;
    tnormalZ.x *= axisSign.z;

    float3 absNormal = abs( geometricNormal );
    tnormalX = blendReorientedNormalMapping( float3( geometricNormal.zy, absNormal.x ), tnormalX );
    tnormalY = blendReorientedNormalMapping( float3( geometricNormal.xz, absNormal.y ), tnormalY );
    tnormalZ = blendReorientedNormalMapping( float3( geometricNormal.xy, absNormal.z ), tnormalZ );

    tnormalY.z *= axisSign.y;
    tnormalX.z *= axisSign.x;
    tnormalZ.z *= axisSign.z;

    return normalize( tnormalX.zyx * triBlend.x + tnormalY.xzy * triBlend.y + tnormalZ.xyz * triBlend.z );
}
