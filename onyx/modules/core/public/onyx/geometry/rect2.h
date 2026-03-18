#pragma once

#include <onyx/geometry/vector2.h>

namespace onyx
{
    template <typename T>
    struct Rect2
    {
        using ScalarT = T;

        Rect2() = default;
        Rect2(const Vector2<ScalarT>& position, const Vector2<ScalarT>& Extents)
            : Position(position)
            , Extents(Extents)
        {
        }

        Rect2(ScalarT positionX, ScalarT positionY, ScalarT extentsX, ScalarT extentsY)
            : Position(positionX, positionY)
            , Extents(extentsX, extentsY)
        {
        }

        Vector2<ScalarT> Position = Vector2<ScalarT>::Zero();
        Vector2<ScalarT> Extents = Vector2<ScalarT>::Zero();

        ScalarT GetLeft() const { return Position[0]; }
        ScalarT GetTop() const { return Position[1]; }
        ScalarT GetRight() const { return Position[0] + Extents[0]; }
        ScalarT GetBottom() const { return Position[1] - Extents[1]; }

        Vector2<ScalarT> GetTopLeft() const { return Position; }
        Vector2<ScalarT> GetTopRight() const { return { GetRight(), GetTop() }; }
        Vector2<ScalarT> GetBottomRight() const { return { GetRight(), GetBottom() }; }
        Vector2<ScalarT> GetBottomLeft() const { return { GetLeft(), GetBottom() }; }
    };
}