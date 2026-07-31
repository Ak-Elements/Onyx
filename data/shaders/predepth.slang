import includes.viewconstants;

struct VSInput {
    float4 position : POSITION;
    float4 normal : NORMAL;
};

struct VSOutput {
    float4 position : SV_Position;
};

struct PushConstants {
    float4x4 ModelMatrix;
    Ptr< ViewConstants > ViewConstants;
};

[ push_constant ]
PushConstants Constants;

[ shader( "vertex" ) ]
VSOutput vertexMain( VSInput input ) {
    VSOutput output;

    output.position = mul(
        Constants.ViewConstants->ProjectionMatrix,
        mul( Constants.ViewConstants->ViewMatrix, mul( Constants.ModelMatrix, float4( input.position.xyz, 1.0 ) ) ) );

    return output;
}

[ shader( "fragment" ) ]
void fragmentMain() {}
