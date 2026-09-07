import includes.color;
import includes.math.functions;

struct PushConstants {
    uint32_t InTextureId;
    uint32_t OutTextureId;

    uint2 InExtents;
    uint2 OutExtents;
};

[ push_constant ]
PushConstants Constants;

[ shader( "compute" ) ]
[ numthreads( 8, 8, 1 ) ]
void computeMain( uint3 threadId: SV_DispatchThreadID ) {
    uint2 outPixelCoordinate = int2( threadId.xy );
    if( ( outPixelCoordinate.x >= Constants.OutExtents.x ) || ( outPixelCoordinate.y >= Constants.OutExtents.y ) ) {
        return;
    }

    DescriptorHandle< Texture2D > sourceTexture = DescriptorHandle< Texture2D >( uint2( Constants.InTextureId, 0 ) );
    DescriptorHandle< RWTexture2D > outTexture = DescriptorHandle< RWTexture2D >( uint2( Constants.OutTextureId, 0 ) );

    // 2x2 box filter
    uint2 sourcePixelCoordinate = outPixelCoordinate * 2;
    uint2 inLastPixelCoordinate = Constants.InExtents - uint2( 1 );
    uint2 coord00 = min( sourcePixelCoordinate + uint2( 0, 0 ), inLastPixelCoordinate );
    uint2 coord10 = min( sourcePixelCoordinate + uint2( 1, 0 ), inLastPixelCoordinate );
    uint2 coord01 = min( sourcePixelCoordinate + uint2( 0, 1 ), inLastPixelCoordinate );
    uint2 coord11 = min( sourcePixelCoordinate + uint2( 1, 1 ), inLastPixelCoordinate );

    float4 texel00 = sourceTexture[ coord00 ];
    float4 texel10 = sourceTexture[ coord10 ];
    float4 texel01 = sourceTexture[ coord01 ];
    float4 texel11 = sourceTexture[ coord11 ];

    float3 normal00 = texel00.xyz * 2.0f - 1.0f;
    float3 normal10 = texel10.xyz * 2.0f - 1.0f;
    float3 normal01 = texel01.xyz * 2.0f - 1.0f;
    float3 normal11 = texel11.xyz * 2.0f - 1.0f;

    float3 average = normal00 + normal10 + normal01 + normal11;
    float3 normal = average * safeReciprocal( length( average ) );

    float4 resultNormal;
    resultNormal.xyz = ( normal * 0.5f ) + 0.5f;
    resultNormal.w = ( texel00.w + texel10.w + texel01.w + texel11.w ) * 0.25f;
    outTexture[ outPixelCoordinate ] = resultNormal;
}
