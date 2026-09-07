
float mod( float x, float y ) {
    return x - y * floor( x / y );
}

float3 mod( float3 x, float y ) {
    return x - y * floor( x / y );
}

float4 mod( float4 x, float y ) {
    return x - y * floor( x / y );
}

float safeReciprocal( float value ) {
    return value == 0.0f ? 0.0f : ( 1.0f / value );
}

float2 safeReciprocal( float2 value ) {
    return float2( safeReciprocal( value.x ), safeReciprocal( value.y ) );
}

float3 safeReciprocal( float3 value ) {
    return float3( safeReciprocal( value.x ), safeReciprocal( value.y ), safeReciprocal( value.z ) );
}

float safeAcos( float x ) {
    return acos( clamp( x, -1.0, 1.0 ) );
}
