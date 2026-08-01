implementing lighting;

import includes.math.constants;
import includes.math.functions;

// Normal Distribution function --------------------------------------
float dGgx( float dotNH, float roughness ) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * ( alpha2 - 1.0 ) + 1.0;
    return alpha2 * safeReciprocal( Pi * denom * denom );
}

// Geometric Shadowing function --------------------------------------
float gSchlicksmithGgx( float dotNL, float dotNV, float roughness ) {
    float r = ( roughness + 1.0 );
    float k = ( r * r ) / 8.0;
    float GL = dotNL * safeReciprocal( dotNL * ( 1.0 - k ) + k );
    float GV = dotNV * safeReciprocal( dotNV * ( 1.0 - k ) + k );
    return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 fSchlick( float3 albedo, float cosTheta, float metallic ) {
    float3 F0 = lerp( Dielectric, albedo, metallic );

    float r = 2 * ( 1 - 0.05 );
    float t = min( max( 2 - r, 0 ), 1 );
    float3 F = F0 + t * ( 1.0 - F0 ) * pow( clamp( 1.0 - cosTheta, 0.0, 1.0 ), 5.0 * r );
    return F;
}

// Specular BRDF composition --------------------------------------------
float3 brdf( float3 L, float3 V, float3 N, float3 radiance, float3 albedo, float metallic, float roughness ) {
    // Precalculate vectors and dot products
    float3 H = normalize( V + L );
    float dotNV = clamp( dot( N, V ), 0.0, 1.0 );
    float dotNL = clamp( dot( N, L ), 0.0, 1.0 );
    float dotNH = clamp( dot( N, H ), 0.0, 1.0 );

    float3 color = float3( 0.0 );

    if( dotNL > 0.0 ) {
        float rroughness = max( 0.05, roughness );
        // D = Normal distribution (Distribution of the microfacets)
        float D = dGgx( dotNH, roughness );
        // G = Geometric shadowing term (Microfacets shadowing)
        float G = gSchlicksmithGgx( dotNL, dotNV, rroughness );
        // F = Fresnel factor (Reflectance depending on angle of incidence)
        float3 F = fSchlick( albedo, dotNV, metallic );

        float3 spec = D * F * G * safeReciprocal( 4.0 * dotNL * dotNV );

        color += spec * dotNL * radiance;
    }

    return color;
}
