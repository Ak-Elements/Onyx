// psrdnoise (c) Stefan Gustavson and Ian McEwan,
// ver. 2021-12-02, published under the MIT license:
// https://github.com/stegu/psrdnoise/
import includes.math.matrix;
import includes.math.functions;

float4 permute( float4 i ) {
    float4 im = mod( i, 289.0 );
    return mod( ( ( im * 34.0 ) + 10.0 ) * im, 289.0 );
}

float psrdnoise( float3 x, float3 period, float alpha, out float3 gradient ) {
    const float3x3 M = float3x3( 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0 );
    const float3x3 Mi = float3x3( -0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, -0.5 );
    float3 uvw = M * x;
    float3 i0 = floor( uvw ), f0 = fract( uvw );
    float3 g_ = step( f0.xyx, f0.yzz ), l_ = 1.0 - g_;
    float3 g = float3( l_.z, g_.xy ), l = float3( l_.xy, g_.z );
    float3 o1 = min( g, l ), o2 = max( g, l );
    float3 i1 = i0 + o1, i2 = i0 + o2, i3 = i0 + float3( 1.0 );
    float3 v0 = mul( Mi, i0 ), v1 = mul( Mi, i1 ), v2 = mul( Mi, i2 ), v3 = mul( Mi, i3 );
    float3 x0 = x - v0, x1 = x - v1, x2 = x - v2, x3 = x - v3;

    if( any( period > float3( 0.0 ) ) ) {
        float4 vx = float4( v0.x, v1.x, v2.x, v3.x );
        float4 vy = float4( v0.y, v1.y, v2.y, v3.y );
        float4 vz = float4( v0.z, v1.z, v2.z, v3.z );
        if( period.x > 0.0 )
            vx = mod( vx, period.x );
        if( period.y > 0.0 )
            vy = mod( vy, period.y );
        if( period.z > 0.0 )
            vz = mod( vz, period.z );
        i0 = floor( M * float3( vx.x, vy.x, vz.x ) + 0.5 );
        i1 = floor( M * float3( vx.y, vy.y, vz.y ) + 0.5 );
        i2 = floor( M * float3( vx.z, vy.z, vz.z ) + 0.5 );
        i3 = floor( M * float3( vx.w, vy.w, vz.w ) + 0.5 );
    }

    float4 hash = permute( permute( permute( float4( i0.z, i1.z, i2.z, i3.z ) ) + float4( i0.y, i1.y, i2.y, i3.y ) ) +
                           float4( i0.x, i1.x, i2.x, i3.x ) );

    float4 theta = hash * 3.883222077;
    float4 sz = hash * -0.006920415 + 0.996539792;
    float4 psi = hash * 0.108705628;
    float4 Ct = cos( theta ), St = sin( theta );
    float4 sz_prime = sqrt( 1.0 - sz * sz );
    float4 gx, gy, gz;
    if( alpha != 0.0 ) {
        float4 px = Ct * sz_prime, py = St * sz_prime, pz = sz;
        float4 Sp = sin( psi ), Cp = cos( psi ), Ctp = St * Sp - Ct * Cp;
        float4 qx = lerp( Ctp * St, Sp, sz ), qy = lerp( -Ctp * Ct, Cp, sz );
        float4 qz = -( py * Cp + px * Sp );
        float4 Sa = float4( sin( alpha ) ), Ca = float4( cos( alpha ) );
        gx = Ca * px + Sa * qx;
        gy = Ca * py + Sa * qy;
        gz = Ca * pz + Sa * qz;
    } else {
        gx = Ct * sz_prime;
        gy = St * sz_prime;
        gz = sz;
    }
    float3 g0 = float3( gx.x, gy.x, gz.x ), g1 = float3( gx.y, gy.y, gz.y );
    float3 g2 = float3( gx.z, gy.z, gz.z ), g3 = float3( gx.w, gy.w, gz.w );
    float4 w = 0.5 - float4( dot( x0, x0 ), dot( x1, x1 ), dot( x2, x2 ), dot( x3, x3 ) );
    w = max( w, 0.0 );
    float4 w2 = w * w, w3 = w2 * w;
    float4 gdotx = float4( dot( g0, x0 ), dot( g1, x1 ), dot( g2, x2 ), dot( g3, x3 ) );
    float n = dot( w3, gdotx );
    float4 dw = -6.0 * w2 * gdotx;
    float3 dn0 = w3.x * g0 + dw.x * x0;
    float3 dn1 = w3.y * g1 + dw.y * x1;
    float3 dn2 = w3.z * g2 + dw.z * x2;
    float3 dn3 = w3.w * g3 + dw.w * x3;
    gradient = 39.5 * ( dn0 + dn1 + dn2 + dn3 );
    return 39.5 * n;
}

