//
// phase.slang
//
// Angular scattering phase functions.
//

implementing atmosphere;

// -----------------------------------------------------------------------------
// Rayleigh phase function
// -----------------------------------------------------------------------------

//
// Molecular scattering.
// Symmetric around 90 degrees.
//

public float rayleighPhaseFunction( float cosTheta ) {
    return ( 3.0 / ( 16.0 * Pi ) ) * ( 1.0 + cosTheta * cosTheta );
}

public float miePhaseFunction( float cosTheta, float mieG ) {
    const float scale = 3.0 / ( 8.0 * Pi );

    float num = ( 1.0 - mieG * mieG ) * ( 1.0 + cosTheta * cosTheta );
    float denom = ( 2.0 + mieG * mieG ) * pow( ( ( 1.0 + mieG * mieG ) - ( 2.0 * mieG * cosTheta ) ), 1.5 );

    return scale * ( num / denom );
}
