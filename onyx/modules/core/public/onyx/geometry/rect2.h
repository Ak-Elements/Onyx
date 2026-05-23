#pragma once

#include <onyx/geometry/vector2.h>

namespace onyx {
template < typename T >
struct Rect2 {
    using ScalarT = T;

    Rect2() = default;
    Rect2( const Vector2< ScalarT >& position, const Vector2< ScalarT >& Extents )
        : Position( position )
        , Extents( Extents ) {}

    Rect2( ScalarT positionX, ScalarT positionY, ScalarT extentsX, ScalarT extentsY )
        : Position( positionX, positionY )
        , Extents( extentsX, extentsY ) {}

    Vector2< ScalarT > Position = Vector2< ScalarT >::zero();
    Vector2< ScalarT > Extents = Vector2< ScalarT >::zero();

    ScalarT getLeft() const { return Position[ 0 ]; }
    ScalarT getTop() const { return Position[ 1 ]; }
    ScalarT getRight() const { return Position[ 0 ] + Extents[ 0 ]; }
    ScalarT getBottom() const { return Position[ 1 ] - Extents[ 1 ]; }

    Vector2< ScalarT > getTopLeft() const { return Position; }
    Vector2< ScalarT > getTopRight() const { return { getRight(), getTop() }; }
    Vector2< ScalarT > getBottomRight() const { return { getRight(), getBottom() }; }
    Vector2< ScalarT > getBottomLeft() const { return { getLeft(), getBottom() }; }
};

using Rect2s16 = Rect2< int16_t >;
using Rect2f32 = Rect2< float32 >;

} // namespace onyx
