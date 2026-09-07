import includes.bindless;
import includes.math.matrix;

struct VSOutput {
    float4 Position : SV_Position;
    float2 Uv;
    nointerpolation uint32_t TextureIndex;
};

[ shader( "vertex" ) ]
VSOutput vertexMain( uint vertexID: SV_VertexID,
                     uint instanceID: SV_InstanceID,
                     uint baseInstance: SV_StartInstanceLocation ) {
    VSOutput output;

    output.Uv = float2( ( vertexID << 1 ) & 2, vertexID & 2 );

    output.Position = float4( output.Uv * 2.0 - 1.0, 0.0, 1.0 );
    output.Position.y = -output.Position.y;

    output.TextureIndex = instanceID + baseInstance;

    return output;
}

struct PushConstants {
    uint32_t TonemapFunction;
};

[ push_constant ]
PushConstants Constants;

float3 TonemapJodieReinhard( float3 c ) {
    // From: https://www.shadertoy.com/view/tdSXzD
    float l = dot( c, float3( 0.2126, 0.7152, 0.0722 ) );
    float3 tc = c / ( c + 1.0 );
    return lerp( c / ( l + 1.0 ), tc, tc );
}

float3 TonemapAces( float3 color ) {
    float3x3 matrix1 = float3x3( 0.59719, 0.35458, 0.04823, 0.07600, 0.90834, 0.01566, 0.02840, 0.13383, 0.83777 );

    float3x3
        matrix2 = float3x3( 1.60475, -0.53108, -0.07367, -0.10208, 1.10813, -0.00605, -0.00327, -0.07276, 1.07602 );

    float3 v = matrix1 * color;
    float3 a = v * ( v + 0.0245786 ) - 0.000090537;
    float3 b = v * ( 0.983729 * v + 0.4329510 ) + 0.238081;
    return clamp( matrix2 * ( a / b ), 0.0, 1.0 );
}

float3 AgxDefaultConstractApproximation( float3 color ) {
    float3 colorSquared = color * color;
    float3 color4 = colorSquared * colorSquared;

    return +15.5 * color4 * colorSquared - 40.14 * color4 * color + 31.96 * color4 - 6.868 * colorSquared * color +
           0.4298 * colorSquared + 0.1191 * color - 0.00232;
}

float3 TonemapAgx( float3 color ) {
    const float3x3 agx_matrix = float3x3( 0.842479062253094,
                                          0.0784335999999992,
                                          0.0792237451477643,
                                          0.0423282422610123,
                                          0.878468636469772,
                                          0.0791661274605434,
                                          0.0423756549057051,
                                          0.0784336,
                                          0.879142973793104 );

    const float min_ev = -12.47393;
    const float max_ev = 4.026069;

    // input color transform
    color = agx_matrix * color;

    // log2 space encoding
    color = clamp( log2( color ), min_ev, max_ev );
    color = ( color - min_ev ) / ( max_ev - min_ev );

    // sigmoid function approximation
    color = AgxDefaultConstractApproximation( color );
    return color;
}

float3 AgxLook( float3 color ) {
    const float3 lw = float3( 0.2126, 0.7152, 0.0722 );
    float luma = dot( color, lw );

    // default
    float3 offset = float3( 0.0 );
    float3 slope = float3( 1.0 );
    float3 power = float3( 1.0 );
    float sat = 1.0;

    if( Constants.TonemapFunction == 4 ) {
        // golden
        slope = float3( 1.0, 0.9, 0.5 );
        power = float3( 0.8 );
        sat = 0.8;
    } else if( Constants.TonemapFunction == 5 ) {
        // punchy
        power = float3( 1.35, 1.35, 1.35 );
        sat = 1.4;
    }

    color = max( color * slope + offset, float3( 0.0f ) );
    color = pow( color, power );
    return luma + sat * ( color - luma );
}

float3 AgxEotf( float3 color ) {
    const float3x3 agx_mat_inv = float3x3( 1.19687900512017,
                                           -0.0980208811401368,
                                           -0.0990297440797205,
                                           -0.0528968517574562,
                                           1.15190312990417,
                                           -0.0989611768448433,
                                           -0.0529716355144438,
                                           -0.0980434501171241,
                                           1.15107367264116 );

    // Undo input transform
    color = agx_mat_inv * color;

    // sRGB IEC 61966-2-1 2.2 Exponent Reference EOTF Display
    // color = pow( color, float3( 2.2 ) );

    return color;
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target0 {
    float4 texture = linearSample( input.TextureIndex, input.Uv );

    float4 color = texture.xyzw;
    color.xyz *= 2.0f * exp2( 2.5 );

    if( Constants.TonemapFunction == 1 ) {
        color.xyz = TonemapJodieReinhard( color.xyz );
        color.xyz = pow( color.xyz, float3( 1.0 / 2.2 ) );
    } else if( Constants.TonemapFunction == 2 ) {
        color.xyz = TonemapAces( color.xyz );
        color.xyz = pow( color.xyz, float3( 1.0 / 2.2 ) );
    } else if( Constants.TonemapFunction >= 3 ) {
        color.xyz = TonemapAgx( color.xyz );
        color.xyz = AgxLook( color.xyz );
        color.xyz = AgxEotf( color.xyz );
    }

    return color;
}
