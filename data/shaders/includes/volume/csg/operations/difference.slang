import includes.volume.terrainsample;

SdfSample getDifference( float4 lhsGradient, float4 rhsGradient ) {
    if( -lhsGradient.w > rhsGradient.w ) {
        return { -lhsGradient.xyz, -lhsGradient.w  };
    }

    return { rhsGradient.xyz, rhsGradient.w };
}
