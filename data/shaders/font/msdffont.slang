
import includes.bindless;
import includes.math.matrix;
import includes.viewconstants;
import includes.textureformat;
import includes.textureformatattribute;

struct VSInput {
    float3 Position : POSITION;
    float2 Uv : NORMAL;

    [ Format( TextureFormat::RGBA_UNORM8 ) ]
    float4 Color : NORMAL;
};

struct VSOutput {
    float4 Position : SV_Position;
    float4 Color : SV_Position;
    float2 Uv : NORMAL;
    nointerpolation uint TextureIndex;
};

struct PushConstants {
    float4x4 Transform;

    Ptr< ViewConstants > ViewConstants;
};

[ push_constant ]
PushConstants Constants;

[ shader( "vertex" ) ]
VSOutput vertexMain( VSInput input, uint instanceID: SV_InstanceID, uint baseInstance: SV_StartInstanceLocation ) {
    VSOutput output;

    output.Color = input.Color;
    output.Uv = input.Uv;
    output.TextureIndex = instanceID + baseInstance;
    output.Position = Constants.ViewConstants.ProjectionMatrix * Constants.ViewConstants.ViewMatrix *
                      Constants.Transform * float4( input.Position, 1.0 );
    return output;
}

// Returns mid value of the 3 color channels
float median( float3 color ) {
    return max( min( color.r, color.g ), min( max( color.r, color.g ), color.b ) );
}

float screenPixelRange( float2 uv ) {
    float2 unitRange = float2( 16.0 ) / float2( 2980, 2980 );
    float2 screenTextureSize = float2( 1.0 ) / fwidth( uv );
    return max( 0.5 * dot( unitRange, screenTextureSize ), 1.0 );
}
[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target {
    float4 backGroundColor = float4( input.Color.xyz, 0 );

    float3 msdSample = linearSample( input.TextureIndex, input.Uv ).xyz;

    float sd = median( msdSample );
    float screenPixelDistance = screenPixelRange( input.Uv ) * ( sd - 0.5 );
    float opacity = clamp( screenPixelDistance + 0.5, 0.0, 1.0 );

    return lerp( backGroundColor, input.Color, opacity );
}
