
float4 fromLinearToSrgb( float4 linearRgb ) {
    bool4 cutoff = linearRgb < float4( 0.0031308 );
    float4 higher = float4( 1.055 ) * pow( linearRgb, float4( 1.0 / 2.4 ) ) - float4( 0.055 );
    float4 lower = linearRgb * float4( 12.92 );

    return lerp( higher, lower, float4( cutoff ) );
}

float4 toLinearFromSrgb( float4 srgb ) {
    bool4 cutoff = srgb < float4( 0.04045 );
    float4 higher = pow( ( srgb + float4( 0.055 ) ) / float4( 1.055 ), float4( 2.4 ) );
    float4 lower = srgb / float4( 12.92 );

    return lerp( higher, lower, float4( cutoff ) );
}
