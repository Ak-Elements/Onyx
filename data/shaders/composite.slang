import includes.bindless;

struct VSOutput
{
    float4 Position : SV_Position;
    float2 Uv;
};

[ shader( "vertex" ) ]
VSOutput vertexMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    output.Uv = float2((vertexID << 1) & 2, vertexID & 2);

    output.Position = float4(output.Uv * 2.0 - 1.0, 0.0, 1.0);
    output.Position.y = -output.Position.y;

    return output;
}

struct PushConstants
{
    uint32_t Textures[8];
    uint Count;
};

float4 alphaBlendColor(float4 src, float4 dst)
{
    float srcFactor = (1.0 - src.a);
    float alpha = src.a + dst.a * srcFactor;
    return float4((src.rgb * src.a + dst.rgb * dst.a * srcFactor) / alpha, alpha);
}

[push_constant]
PushConstants Constants;

[ shader( "fragment" ) ]
float4 fragmentMain(  VSOutput input ) : SV_Target0 {
    float4 color = linearSample( Constants.Textures[ 0 ], input.Uv );
    float4 fontTex = linearSample( Constants.Textures[ 2 ], input.Uv );
    return alphaBlendColor( fontTex, color );
}
