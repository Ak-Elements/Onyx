import includes.bindless;
import includes.textureformat;
import includes.textureformatattribute;

struct PushConstants
{
    float2 Scale;
    float2 Translation;
};

[ push_constant ]
PushConstants Constants;

struct VSInput
{
    float2 Position;
    float2 Uv;

    [Format( TextureFormat::RGBA_UNORM8 )]
    float4 Color;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 Uv;
    float4 Color;
    
    nointerpolation uint TextureIndex;
};

[shader("vertex")]
VSOutput vertexMain( VSInput input, uint instanceID : SV_InstanceID, uint baseInstance : SV_StartInstanceLocation )
{
    VSOutput output;

    output.Uv = input.Uv;
    output.Color = input.Color;
    output.TextureIndex = instanceID + baseInstance;

    output.Position = float4( input.Position * Constants.Scale + Constants.Translation, 0.0, 1.0 );

    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target
{
    float4 sampledTexture = linearSample( input.TextureIndex, input.Uv );
    return input.Color * sampledTexture;
}
