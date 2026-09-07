bool getUnion( float4 lhsGradient, float4 rhsGradient ) {
    if( lhsGradient.w < rhsGradient.w ) {
        return true;
    }

    return false;
}

float4 getUnionSmooth( float4 lhsGradient, float4 rhsGradient, float smoothness ) {
    smoothness *= 4.0;
    float h = max( smoothness - abs( lhsGradient.w - rhsGradient.w ), 0.0 ) / ( 2.0 * smoothness );
    return float4( lerp( lhsGradient.xyz, rhsGradient.xyz, ( lhsGradient.w < rhsGradient.w ) ? h : 1.0 - h ),
                   min( lhsGradient.w, rhsGradient.w ) - h * h * smoothness );
}
