#pragma once

namespace onyx {

template < typename T > requires std::is_floating_point_v< T >
T psrdnoise( Vector2< T > x, Vector2< T > period, T alpha, Vector2< T >& outGradient ) {
    auto mod = []( Vector3< T > x, T y ) -> Vector3< T > {
        return x - y * Vector3< T >{ std::floor( x.X / y ), std::floor( x.Y / y ), std::floor( x.Z / y ) };
    };

    auto mod2 = []( T x, T y ) -> T { return x - y * std::floor( x / y ); };

    Vector2< T > uv = Vector2< T >( x.X + x.Y * T{ 0.5 }, x.Y );
    Vector2< T > i0{ std::floor( uv.X ), std::floor( uv.Y ) };
    Vector2< T > f0{ uv.X - i0.X, uv.Y - i0.Y };
    T cmp = f0.X < f0.Y ? 0.0f : 1.0f;
    Vector2< T > o1 = Vector2< T >( cmp, T{ 1.0 } - cmp );
    Vector2< T > i1 = i0 + o1, i2 = i0 + T{ 1.0 };
    Vector2< T > v0 = Vector2< T >( i0.X - i0.Y * T{ 0.5 }, i0.Y );
    Vector2< T > v1 = Vector2< T >( v0.X + o1.X - o1.Y * T{ 0.5 }, v0.Y + o1.Y );
    Vector2< T > v2 = Vector2< T >( v0.X + T{ 0.5 }, v0.Y + T{ 1.0 } );
    Vector2< T > x0 = x - v0, x1 = x - v1, x2 = x - v2;
    Vector3< T > iu, iv, xw, yw;
    if( ( period.X > T{ 0.0f } ) || ( period.Y > T{ 0.0f } ) ) {
        xw = Vector3< T >( v0.X, v1.X, v2.X );
        yw = Vector3< T >( v0.Y, v1.Y, v2.Y );
        if( period.X > T{ 0.0 } )
            xw = mod( Vector3< T >( v0.X, v1.X, v2.X ), period.X );
        if( period.Y > T{ 0.0 } )
            yw = mod( Vector3< T >( v0.Y, v1.Y, v2.Y ), period.Y );
        iu = { std::floor( xw.X + T{ 0.5 } * yw.X + T{ 0.5 } ),
               std::floor( xw.Y + T{ 0.5 } * yw.Y + T{ 0.5 } ),
               std::floor( xw.Z + T{ 0.5 } * yw.Z + T{ 0.5 } ) };
        iv = { std::floor( yw.X + T{ 0.5 } ), std::floor( yw.Y + T{ 0.5 } ), std::floor( yw.Z + T{ 0.5 } ) };
    } else {
        iu = Vector3< T >( i0.X, i1.X, i2.X );
        iv = Vector3< T >( i0.Y, i1.Y, i2.Y );
    }
    Vector3< T > hash = mod( iu, T{ 289.0 } );
    hash = Vector3< T >{ mod2( ( hash.X * T{ 51.0 } + T{ 2.0 } ) * hash.X + iv.X, 289.0 ),
                         mod2( ( hash.Y * T{ 51.0 } + T{ 2.0 } ) * hash.Y + iv.Y, 289.0 ),
                         mod2( ( hash.Z * T{ 51.0 } + T{ 2.0 } ) * hash.Z + iv.Z, 289.0 ) };
    hash = Vector3< T >{ mod2( ( hash.X * T{ 34.0 } + T{ 10.0 } ) * hash.X, 289.0 ),
                         mod2( ( hash.Y * T{ 34.0 } + T{ 10.0 } ) * hash.Y, 289.0 ),
                         mod2( ( hash.Z * T{ 34.0 } + T{ 10.0 } ) * hash.Z, 289.0 ) };
    Vector3< T > psi = hash * T{ 0.07482 } + Vector3< T >( alpha );
    Vector3< T > gx{ std::cos( psi.X ), std::cos( psi.Y ), std::cos( psi.Z ) };
    Vector3< T > gy{ std::sin( psi.X ), std::sin( psi.Y ), std::sin( psi.Z ) };
    Vector2< T > g0 = Vector2< T >( gx.X, gy.X );
    Vector2< T > g1 = Vector2< T >( gx.Y, gy.Y );
    Vector2< T > g2 = Vector2< T >( gx.Z, gy.Z );
    Vector3< T > w = T{ 0.8 } - Vector3< T >( x0.dot( x0 ), x1.dot( x1 ), x2.dot( x2 ) );
    w = { std::max( w.X, T{ 0.0 } ), std::max( w.Y, T{ 0.0 } ), std::max( w.Z, T{ 0.0 } ) };
    Vector3< T > w2 = { w.X * w.X, w.Y * w.Y, w.Z * w.Z };
    Vector3< T > w4 = { w2.X * w2.X, w2.Y * w2.Y, w2.Z * w2.Z };
    Vector3< T > gdotx = Vector3< T >( g0.dot( x0 ), g1.dot( x1 ), g2.dot( x2 ) );
    T n = w4.dot( gdotx );
    Vector3< T > w3{ w2.X * w.X, w2.Y * w.Y, w2.Z * w.Z };
    Vector3< T > dw{ T{ -8.0 } * w3.X * gdotx.X, T{ -8.0 } * w3.Y * gdotx.Y, T{ -8.0 } * w3.Z * gdotx.Z };
    Vector2< T > dn0 = w4.X * g0 + dw.X * x0;
    Vector2< T > dn1 = w4.Y * g1 + dw.Y * x1;
    Vector2< T > dn2 = w4.Z * g2 + dw.Z * x2;
    outGradient = T{ 10.9 } * ( dn0 + dn1 + dn2 );
    return T{ 10.9 } * n;
}
template < typename T > requires std::is_floating_point_v< T >
T psrdnoise( Vector2< T > x, Vector2< T >& outGradient ) {
    return psrdnoise( x, Vector2< T >::zero(), T{ 0 }, outGradient );
}
} // namespace onyx
