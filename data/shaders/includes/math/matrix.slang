
float2 operator*( float2x2 matrix, float2 vector ) {
    return mul( matrix, vector );
}

float3 operator*( float3x3 matrix, float3 vector ) {
    return mul( matrix, vector );
}

float4 operator*( float4x4 matrix, float4 vector ) {
    return mul( matrix, vector );
}

