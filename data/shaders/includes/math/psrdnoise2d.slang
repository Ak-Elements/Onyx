// psrdnoise (c) Stefan Gustavson and Ian McEwan,
// ver. 2021-12-02, published under the MIT license:
// https://github.com/stegu/psrdnoise/
import includes.math.functions;

float psrdnoise( float2 x, float2 period, float alpha, out float2 gradient ) {
    float2 uv = float2( x.x + x.y * 0.5, x.y );
    float2 i0 = floor( uv ), f0 = fract( uv );
    float cmp = step( f0.y, f0.x );
    float2 o1 = float2( cmp, 1.0 - cmp );
    float2 i1 = i0 + o1, i2 = i0 + 1.0;
    float2 v0 = float2( i0.x - i0.y * 0.5, i0.y );
    float2 v1 = float2( v0.x + o1.x - o1.y * 0.5, v0.y + o1.y );
    float2 v2 = float2( v0.x + 0.5, v0.y + 1.0 );
    float2 x0 = x - v0, x1 = x - v1, x2 = x - v2;
    float3 iu, iv, xw, yw;
    if( any( period > float2( 0.0 ) ) ) {
        xw = float3( v0.x, v1.x, v2.x );
        yw = float3( v0.y, v1.y, v2.y );
        if( period.x > 0.0 )
            xw = mod( float3( v0.x, v1.x, v2.x ), period.x );
        if( period.y > 0.0 )
            yw = mod( float3( v0.y, v1.y, v2.y ), period.y );
        iu = floor( xw + 0.5 * yw + 0.5 );
        iv = floor( yw + 0.5 );
    } else {
        iu = float3( i0.x, i1.x, i2.x );
        iv = float3( i0.y, i1.y, i2.y );
    }
    float3 hash = mod( iu, 289.0 );
    hash = mod( ( hash * 51.0 + 2.0 ) * hash + iv, 289.0 );
    hash = mod( ( hash * 34.0 + 10.0 ) * hash, 289.0 );
    float3 psi = hash * 0.07482 + alpha;
    float3 gx = cos( psi );
    float3 gy = sin( psi );
    float2 g0 = float2( gx.x, gy.x );
    float2 g1 = float2( gx.y, gy.y );
    float2 g2 = float2( gx.z, gy.z );
    float3 w = 0.8 - float3( dot( x0, x0 ), dot( x1, x1 ), dot( x2, x2 ) );
    w = max( w, 0.0 );
    float3 w2 = w * w;
    float3 w4 = w2 * w2;
    float3 gdotx = float3( dot( g0, x0 ), dot( g1, x1 ), dot( g2, x2 ) );
    float n = dot( w4, gdotx );
    float3 w3 = w2 * w;
    float3 dw = -8.0 * w3 * gdotx;
    float2 dn0 = w4.x * g0 + dw.x * x0;
    float2 dn1 = w4.y * g1 + dw.y * x1;
    float2 dn2 = w4.z * g2 + dw.z * x2;
    gradient = 10.9 * ( dn0 + dn1 + dn2 );
    return 10.9 * n;
}
