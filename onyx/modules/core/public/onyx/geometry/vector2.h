#pragma once

#include <onyx/geometry/vector.h>
namespace Onyx
{
    template <typename Scalar>
    struct Vector3;

    template <typename Scalar>
    struct Vector2;

    template <typename Scalar>
    struct Vector2
    {
    public:
        using ScalarT = Scalar;
        using FloatingPointScalarT = typename decltype([] {
            if constexpr (sizeof(ScalarT) <= sizeof(onyxF32))
                return std::type_identity<onyxF32>();
            else
                return std::type_identity<onyxF64>();
            }())::type;

        using DotT = typename decltype([] {
            if constexpr (std::is_floating_point_v<ScalarT>)
                return std::type_identity<onyxF64>();
            else if constexpr (std::is_signed_v<ScalarT>)
                return std::type_identity<onyxS64>();
            else
                return std::type_identity<onyxU64>();
            }())::type;

        static constexpr Vector2 Zero() { return Vector2(0, 0); }
        static constexpr Vector2 X_Unit() { return Vector2(1, 0); }
        static constexpr Vector2 Y_Unit() { return Vector2(0, 1); }

        constexpr Vector2()
            : X(ScalarT{})
            , Y(ScalarT{})
        {
        }

        constexpr explicit Vector2(ScalarT scalar)
            : X(scalar)
            , Y(scalar)
        {
        }

        constexpr Vector2(ScalarT x, ScalarT y)
            : X(x)
            , Y(y)
        {
        }

        template <typename OtherScalarT>
        constexpr explicit Vector2(const Vector2<OtherScalarT>& vec2)
            : X(numeric_cast<ScalarT>(vec2.X))
            , Y(numeric_cast<ScalarT>(vec2.Y))
        {
            
        }
        constexpr explicit Vector2(const Vector3<ScalarT>& vec3);

        constexpr ScalarT& operator[] (onyxU32 index) { ONYX_ASSERT(index < 2, "Axis index out of bounds."); return *(&X + index); }
        constexpr ScalarT operator[] (onyxU32 index) const { ONYX_ASSERT(index < 2, "Axis index out of bounds."); return *(&X + index); }

        constexpr Vector2 operator-() const requires (std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>) { return Vector2(-X, -Y); }

        constexpr void Inverse() requires (std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            X = -X;
            Y = -Y;
        }

        constexpr FloatingPointScalarT Length() const
        {
            using std::sqrt;
            return numeric_cast<FloatingPointScalarT>(std::sqrt(LengthSquared()));
        }

        constexpr DotT LengthSquared() const
        {
            return Dot(*this);
        }

        constexpr void Set(ScalarT x, ScalarT y)
        {
            X = x;
            Y = y;
        }

        constexpr void Normalize() requires std::is_floating_point_v<ScalarT>
        {
            const FloatingPointScalarT length = Length();

            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");

            const FloatingPointScalarT invLength = (1 / length);
            X *= invLength;
            Y *= invLength;
        }

        constexpr Vector2<FloatingPointScalarT> Normalized() const
        {
            const FloatingPointScalarT length = Length();
            
            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");
            
            const FloatingPointScalarT invLength = (1 / length);
            return Vector2<FloatingPointScalarT>(X * invLength, Y * invLength);
        }

        constexpr DotT Dot(const Vector2& rhs) const
        {
            return numeric_cast<DotT>(X) * numeric_cast<DotT>(rhs.X) +
                numeric_cast<DotT>(Y) * numeric_cast<DotT>(rhs.Y);
        }

        constexpr auto Cross(const Vector2& rhs) const
        {
            using OutT = typename decltype([] {
                if constexpr (std::is_floating_point_v<ScalarT>)
                    return std::type_identity<onyxF64>();
                else
                    return std::type_identity<onyxS64>();
                }())::type;

            // use static cast as we can guarantee that the component will fit
            return (static_cast<OutT>(X) * static_cast<OutT>(rhs.Y)) -
                (static_cast<OutT>(Y) * static_cast<OutT>(rhs.X));
        }

        constexpr DotT operator|(const Vector2& rhs) const
        {
            return Dot(rhs);
        }

        constexpr void operator+=(const ScalarT scalar)
        {
            ONYX_ASSERT(IsAdditionSafe(X, scalar));
            ONYX_ASSERT(IsAdditionSafe(Y, scalar));

            X += scalar;
            Y += scalar;
        }

        constexpr void operator+=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsAdditionSafe(X, rhs.X));
            ONYX_ASSERT(IsAdditionSafe(Y, rhs.Y));

            X += rhs.X;
            Y += rhs.Y;
        }

        constexpr void operator-=(const ScalarT scalar)
        {
            ONYX_ASSERT(IsSubtractionSafe(X, scalar));
            ONYX_ASSERT(IsSubtractionSafe(Y, scalar));

            X -= scalar;
            Y -= scalar;
        }

        constexpr void operator-=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsSubtractionSafe(X, rhs.X));
            ONYX_ASSERT(IsSubtractionSafe(Y, rhs.Y));

            X -= rhs.X;
            Y -= rhs.Y;
        }

        constexpr void operator*=(const ScalarT scalar)
        {
            //ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));

            X *= scalar;
            Y *= scalar;
        }

        constexpr void operator*=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsMultiplicationSafe(X, rhs.X));
            ONYX_ASSERT(IsMultiplicationSafe(Y, rhs.Y));

            X *= rhs.X;
            Y *= rhs.Y;
        }

        constexpr void operator/=(const ScalarT scalar)
        {
            const FloatingPointScalarT invScalar = 1 / numeric_cast<FloatingPointScalarT>(scalar);

            ONYX_ASSERT(IsMultiplicationSafe(X, invScalar));
            ONYX_ASSERT(IsMultiplicationSafe(Y, invScalar));

            X = numeric_cast<ScalarT>(X * invScalar);
            Y = numeric_cast<ScalarT>(Y * invScalar);
        }

        constexpr void operator/=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsDivisionSafe(X, rhs.X));
            ONYX_ASSERT(IsDivisionSafe(Y, rhs.Y));

            X /= rhs.X;
            Y /= rhs.Y;
        }

        constexpr Vector2 operator+(ScalarT scalar) const
        {
            ONYX_ASSERT(IsAdditionSafe(X, scalar));
            ONYX_ASSERT(IsAdditionSafe(Y, scalar));

            return Vector2(X + scalar, Y + scalar);
        }

        constexpr Vector2 operator+(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsAdditionSafe(X, rhs.X));
            ONYX_ASSERT(IsAdditionSafe(Y, rhs.Y));

            return Vector2(X + rhs.X, Y + rhs.Y);
        }

        constexpr Vector2 operator-(ScalarT scalar) const
        {
            ONYX_ASSERT(IsSubtractionSafe(X, scalar));
            ONYX_ASSERT(IsSubtractionSafe(Y, scalar));

            return Vector2(X - scalar, Y - scalar);
        }

        constexpr Vector2 operator-(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsSubtractionSafe(X, rhs.X));
            ONYX_ASSERT(IsSubtractionSafe(Y, rhs.Y));

            return Vector2(X - rhs.X, Y - rhs.Y);
        }

        constexpr Vector2 operator*(ScalarT scalar) const
        {
            //ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));

            return Vector2(X * scalar, Y * scalar);
        }

        constexpr Vector2 operator*(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsMultiplicationSafe(X, rhs.X));
            ONYX_ASSERT(IsMultiplicationSafe(Y, rhs.Y));

            return Vector2(X * rhs.X, Y * rhs.Y);
        }

        constexpr Vector2 operator/(ScalarT scalar) const
        {
            ONYX_ASSERT(IsDivisionSafe(X, scalar));
            ONYX_ASSERT(IsDivisionSafe(Y, scalar));

            return Vector2(X / scalar, Y / scalar);
        }

        constexpr Vector2 operator/(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsDivisionSafe(X, rhs.X));
            ONYX_ASSERT(IsDivisionSafe(X, rhs.Y));

            return Vector2(X / rhs.X, Y / rhs.Y);
        }

        constexpr void Rotate(ScalarT radians) requires std::is_floating_point_v<ScalarT>
        {
            Scalar cosAlpha = std::cos(radians);
            Scalar sinAlpha = std::sin(radians);

            Scalar rotatedX = cosAlpha * X - sinAlpha * Y;
            Scalar rotatedY = sinAlpha * X + cosAlpha * Y;
            X = rotatedX;
            Y = rotatedY;
        }

        constexpr auto Rotated(onyxF64 radians) const requires(std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            FloatingPointScalarT rad = numeric_cast<FloatingPointScalarT>(radians);
            FloatingPointScalarT cosAlpha = std::cos(rad);
            FloatingPointScalarT sinAlpha = std::sin(rad);

            return Vector2<FloatingPointScalarT>
            {
                numeric_cast<FloatingPointScalarT>(cosAlpha * X - sinAlpha * Y),
                numeric_cast<FloatingPointScalarT>(sinAlpha * X + cosAlpha * Y)
            };
        }

        constexpr auto RotatedTo(Vector2 to) const
        {
            const Vector2<FloatingPointScalarT>& fromNormalized = Normalized();
            const Vector2<FloatingPointScalarT>& toNormalized = to.Normalized();

            FloatingPointScalarT cosAlpha = numeric_cast<FloatingPointScalarT>(toNormalized.Dot(fromNormalized));
            FloatingPointScalarT sinAlpha = fromNormalized.X * toNormalized.Y - fromNormalized.Y * toNormalized.X;

            return Vector2<FloatingPointScalarT>
            {
                numeric_cast<FloatingPointScalarT>(cosAlpha* X - sinAlpha * Y),
                numeric_cast<FloatingPointScalarT>(sinAlpha* X + cosAlpha * Y)
            };
        }

        constexpr bool operator==(const Vector2& rhs) const { return IsEqual(rhs); }
        constexpr bool operator!=(const Vector2& rhs) const { return IsEqual(rhs) == false; }

        template <typename T>
        constexpr VectorComponentMask operator<(const Vector2<T>& rhs) const
        {
            return ((X < rhs.X) ? VectorComponentMask::X : VectorComponentMask::None) |
                    ((Y < rhs.Y) ? VectorComponentMask::Y : VectorComponentMask::None);
        }

        template <typename T>
        constexpr VectorComponentMask operator<=(const Vector2<T>& rhs) const
        {
            return ((X <= rhs.X) ? VectorComponentMask::X : VectorComponentMask::None) |
                ((Y <= rhs.Y) ? VectorComponentMask::Y : VectorComponentMask::None);
        }


        template <typename T>
        constexpr VectorComponentMask operator>(const Vector2<T>& rhs) const
        {
            return((X > rhs.X) ? VectorComponentMask::X : VectorComponentMask::None) |
                ((Y > rhs.Y) ? VectorComponentMask::Y : VectorComponentMask::None);
        }

        template <typename T>
        constexpr VectorComponentMask operator>=(const Vector2<T>& rhs) const
        {
            return((X >= rhs.X) ? VectorComponentMask::X : VectorComponentMask::None) |
                ((Y >= rhs.Y) ? VectorComponentMask::Y : VectorComponentMask::None);
        }

        constexpr bool IsEqual(const Vector2& rhs) const
        {
            return (Onyx::IsEqual(X, rhs.X)) &&
                (Onyx::IsEqual(Y, rhs.Y));
        }

        constexpr bool IsEqual(const Vector2& rhs, Scalar epsilon) const requires std::is_floating_point_v<Scalar>
        {
            return (Onyx::IsEqual(X, rhs.X, epsilon)) &&
                (Onyx::IsEqual(Y, rhs.Y, epsilon));
        }

        constexpr bool IsZero() const
        {
            return Onyx::IsZero(X) && Onyx::IsZero(Y);
        }

        constexpr bool IsZero(ScalarT epsilon) const requires std::is_floating_point_v<Scalar>
        {
            return Onyx::IsZero(X, epsilon) && Onyx::IsZero(Y, epsilon);
        }

        constexpr Vector2 GetPerpendicularClockwise() const
        {
            return Vector2(Y, -X);
        }

        constexpr Vector2 GetPerpendicularCounterClockwise() const
        {
            return Vector2(-Y, X);
        }

        ScalarT X;
        ScalarT Y;
    };

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator+(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs.X));
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs.Y));

        return Vector2<ScalarT>(lhs + rhs.X, lhs + rhs.Y);
    }

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator-(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs.X));
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs.Y));

        return Vector2<ScalarT>(lhs - rhs.X, lhs - rhs.Y);
    }

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator*(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.X));
        ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.Y));

        return Vector2<ScalarT>(lhs * rhs.X, lhs * rhs.Y);
    }

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator/(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs.X));
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs.Y));

        return Vector2<ScalarT>(lhs / rhs.X, lhs / rhs.Y);
    }
}
