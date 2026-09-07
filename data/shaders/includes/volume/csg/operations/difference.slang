float4 getDifference( float4 lhsGradient, float4 rhsGradient ) {
    if( -lhsGradient.w > rhsGradient.w ) {
        return -lhsGradient;
    }

    return rhsGradient;
}
