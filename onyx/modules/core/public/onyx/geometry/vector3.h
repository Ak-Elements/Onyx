#pragma once

#include <onyx/geometry/vector2.h>

namespace Onyx
{
    template <typename Scalar>
    struct Vector4;

    template <typename Scalar>
    struct Vector3;

    template <typename Scalar>
    struct Bivector3
    {
    public:
        static constexpr Bivector3 XY_Unit();
        static constexpr Bivector3 ZX_Unit();
        static constexpr Bivector3 YZ_Unit();

        constexpr Bivector3() = default;

        constexpr explicit Bivector3(Scalar scalar)
            : m_Components{ scalar, scalar, scalar }
        {
        }

        constexpr Bivector3(Scalar x, Scalar y, Scalar z)
            : m_Components{ x, y, z }
        {
        }

        template <typename U>
        constexpr Bivector3(const Bivector3<U>& vec)
            : m_Components{ vec[0], vec[1], vec[2] }
        {
        }

        constexpr Scalar& operator[] (onyxU32 index) { ONYX_ASSERT(index < 3, "Axis index out of bounds."); return m_Components[index]; }
        constexpr Scalar operator[] (onyxU32 index) const { ONYX_ASSERT(index < 3, "Axis index out of bounds."); return m_Components[index]; }

        constexpr bool operator==(const Bivector3& rhs) const
        {
            return IsEqual(m_Components[0], rhs.m_Components[0]) &&
                   IsEqual(m_Components[1], rhs.m_Components[1]) &&
                   IsEqual(m_Components[2], rhs.m_Components[2]);
        }

    private:
        Scalar m_Components[3] {Scalar(0), Scalar(0) , Scalar(0)};
    };

    template <typename Scalar>
    struct Vector3
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

        static constexpr Vector3 Zero() { return Vector3(0, 0, 0); }
        static constexpr Vector3 X_Unit() { return Vector3(1, 0, 0); }
        static constexpr Vector3 Y_Unit() { return Vector3(0, 1, 0); }
        static constexpr Vector3 Z_Unit() { return Vector3(0, 0, 1); }

        ScalarT X;
        ScalarT Y;
        ScalarT Z;

        constexpr Vector3()
            : Vector3(0, 0, 0)
        {
        }

        explicit constexpr Vector3(ScalarT scalar)
            : X(scalar)
            , Y(scalar)
            , Z(scalar)
        {
        }

        constexpr Vector3(ScalarT x, ScalarT y, ScalarT z)
            : X(x)
            , Y(y)
            , Z(z)
        {
        }

        constexpr Vector3(const Vector2<ScalarT>& vec2, ScalarT z)
            : X(vec2[0])
            , Y(vec2[1])
            , Z(z)
        {
        }

        template <typename OtherScalarT>
        explicit constexpr Vector3(const Vector3<OtherScalarT> otherVec)
            : X(otherVec.X)
            , Y(otherVec.Y)
            , Z(otherVec.Z)
        {
        }

        constexpr explicit Vector3(const Vector4<ScalarT>& vec4);

        constexpr ScalarT& operator[] (onyxU32 index)
        {
            ONYX_ASSERT(index < 3, "Axis index out of bounds.");
            return *(&X + index);
        }

        constexpr ScalarT operator[] (onyxU32 index) const
        {
            ONYX_ASSERT(index < 3, "Axis index out of bounds.");
            return *(&X + index);
        }

        constexpr Vector3 operator-() const requires (std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>) { return Vector3(-X, -Y, -Z); }

        constexpr void Inverse() requires (std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            X = -X;
            Y = -Y;
            Z = -Z;
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

        constexpr void Normalize() requires std::is_floating_point_v<Scalar>
        {
            const onyxF64 length = Length();
            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");

            const FloatingPointScalarT invLength = numeric_cast<FloatingPointScalarT>(1 / length);
            X = numeric_cast<Scalar>(X * invLength);
            Y = numeric_cast<Scalar>(Y * invLength);
            Z = numeric_cast<Scalar>(Z * invLength);
        }
        
        constexpr Vector3<FloatingPointScalarT> Normalized() const
        {
            const FloatingPointScalarT length = Length();
            if (Onyx::IsZero(length))
                return Vector3<FloatingPointScalarT>::Zero();

            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");

            const FloatingPointScalarT invLength = (1 / length);
            return
            {
                numeric_cast<FloatingPointScalarT>(X * invLength),
                numeric_cast<FloatingPointScalarT>(Y * invLength),
                numeric_cast<FloatingPointScalarT>(Z * invLength)
            };
        }

        constexpr DotT Dot(const Vector3& rhs) const
        {
            return numeric_cast<DotT>(X) * numeric_cast<DotT>(rhs.X) +
                numeric_cast<DotT>(Y) * numeric_cast<DotT>(rhs.Y) +
                numeric_cast<DotT>(Z) * numeric_cast<DotT>(rhs.Z);
        }

        constexpr Vector3 Cross(const Vector3& rhs) const
        {
            return
            {
                Y * rhs.Z - rhs.Y * Z,
                Z * rhs.X - rhs.Z * X,
                X * rhs.Y - rhs.X * Y
            };
        }

        constexpr void Rotate(onyxF64 radians, const Bivector3<ScalarT>& bivector) requires std::is_floating_point_v<Scalar>
        {
            // TODO: Replace with Rotor3 (game project)
            ScalarT sina = numeric_cast<ScalarT>(std::sin(radians / 2.0));
            ScalarT a = numeric_cast<ScalarT>(std::cos(radians / 2.0));
            // the left side of the products have b a, not a b
            ScalarT b01 = -sina * bivector[0];
            ScalarT b02 = -sina * bivector[1];
            ScalarT b12 = -sina * bivector[2];

            // q = P x
            Vector3 q;
            q[0] = a * X + Y * b01 + Z * b02;
            q[1] = a * Y - X * b01 + Z * b12;
            q[2] = a * Z - X * b02 - Y * b12;

            ScalarT q012 = X * b12 - Y * b02 + Z * b01; // trivector

            // r = q P*
            X = a * q[0] + q[1] * b01 + q[2] * b02 + q012 * b12;
            Y = a * q[1] - q[0] * b01 - q012 * b02 + q[2] * b12;
            Z = a * q[2] + q012 * b01 - q[0] * b02 - q[1] * b12;

            // trivector part of the result is always zero!
        }

        constexpr auto RotatedAroundX(onyxF64 radians) const requires (std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            return Rotated(radians, Bivector3<FloatingPointScalarT>::YZ_Unit());
        }

        constexpr auto RotatedAroundY(onyxF64 radians) const requires(std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            return Rotated(radians, Bivector3<FloatingPointScalarT>::ZX_Unit());
        }

        constexpr auto RotatedAroundZ(onyxF64 radians) const requires(std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            return Rotated(radians, Bivector3<FloatingPointScalarT>::XY_Unit());
        }

        constexpr Vector3<FloatingPointScalarT> Rotated(onyxF64 radians, const Bivector3<FloatingPointScalarT>& bivector) const requires(std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>)
        {
            const FloatingPointScalarT halfRadians = numeric_cast<FloatingPointScalarT>(radians / 2.0);
            FloatingPointScalarT sina = std::sin(halfRadians);
            FloatingPointScalarT a = std::cos(halfRadians);

            // the left side of the products have b a, not a b
            FloatingPointScalarT b01 = -sina * bivector[0];
            FloatingPointScalarT b02 = -sina * bivector[1];
            FloatingPointScalarT b12 = -sina * bivector[2];

            // q = P x
            const FloatingPointScalarT q0 = a * X + Y * b01 + Z * b02;
            const FloatingPointScalarT q1 = a * Y - X * b01 + Z * b12;
            const FloatingPointScalarT q2 = a * Z - X * b02 - Y * b12;

            FloatingPointScalarT q012 = X * b12 - Y * b02 + Z * b01; // trivector

            // r = q P*
            // trivector part of the result is always zero!
            return Vector3<FloatingPointScalarT>
            {
                numeric_cast<FloatingPointScalarT>(a * q0 + q1 * b01 + q2 * b02 + q012 * b12),
                numeric_cast<FloatingPointScalarT>(a * q1 - q0 * b01 - q012 * b02 + q2 * b12),
                numeric_cast<FloatingPointScalarT>(a * q2 + q012 * b01 - q0 * b02 - q1 * b12),
            };
        }

        constexpr Vector3<FloatingPointScalarT> RotatedTo(Vector3 to) const
        {
            const Vector3<FloatingPointScalarT>& fromNormalized = Normalized();
            const Vector3<FloatingPointScalarT>& toNormalized = to.Normalized();

            FloatingPointScalarT a = static_cast<FloatingPointScalarT>(1 + toNormalized.Dot(fromNormalized));
            Bivector3<FloatingPointScalarT> bivector = toNormalized.Wedge(fromNormalized);

            FloatingPointScalarT lengthSquared = (a * a) + (bivector[0] * bivector[0]) + (bivector[1] * bivector[1]) + (bivector[2] * bivector[2]);
            FloatingPointScalarT length = std::sqrt(lengthSquared);

            a /= length;
            bivector[0] /= length;
            bivector[1] /= length;
            bivector[2] /= length;

            const FloatingPointScalarT b01 = bivector[0];
            const FloatingPointScalarT b02 = bivector[1];
            const FloatingPointScalarT b12 = bivector[2];

            const FloatingPointScalarT q0 = a * X + Y * b01 + Z * b02;
            const FloatingPointScalarT q1 = a * Y - X * b01 + Z * b12;
            const FloatingPointScalarT q2 = a * Z - X * b02 - Y * b12;

            FloatingPointScalarT q012 = X * b12 - Y * b02 + Z * b01; // trivector

            // r = q P*
            // trivector part of the result is always zero!
            return Vector3<FloatingPointScalarT>
            {
                numeric_cast<FloatingPointScalarT>(a* q0 + q1 * b01 + q2 * b02 + q012 * b12),
                numeric_cast<FloatingPointScalarT>(a* q1 - q0 * b01 - q012 * b02 + q2 * b12),
                numeric_cast<FloatingPointScalarT>(a* q2 + q012 * b01 - q0 * b02 - q1 * b12),
            };
        }

        constexpr Bivector3<ScalarT> Wedge(const Vector3& rhs) const
        {
            return Bivector3<ScalarT>
            {
                X * rhs.Y - Y * rhs.X, // XY
                X * rhs.Z - Z * rhs.X, // XZ
                Y * rhs.Z - Z * rhs.Y  // YZ
            };
        }

        constexpr DotT operator|(const Vector3& rhs) const
        {
            return Dot(rhs);
        }

        constexpr void operator+=(const Vector3& rhs)
        {
            //ONYX_ASSERT(IsAdditionSafe(X, rhs.X));
            //ONYX_ASSERT(IsAdditionSafe(Y, rhs.Y));
            //ONYX_ASSERT(IsAdditionSafe(Z, rhs.Z));

            X += rhs.X;
            Y += rhs.Y;
            Z += rhs.Z;
        }

        constexpr void operator-=(const Vector3& rhs)
        {
            ONYX_ASSERT(IsSubtractionSafe(X, rhs.X));
            ONYX_ASSERT(IsSubtractionSafe(Y, rhs.Y));
            ONYX_ASSERT(IsSubtractionSafe(Z, rhs.Z));

            X -= rhs.X;
            Y -= rhs.Y;
            Z -= rhs.Z;
        }

        constexpr void operator*=(const ScalarT scalar)
        {
            /*ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
            ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));
            ONYX_ASSERT(IsMultiplicationSafe(Z, scalar));*/

            X *= scalar;
            Y *= scalar;
            Z *= scalar;
        }

        constexpr void operator/=(const ScalarT scalar)
        {
            const FloatingPointScalarT invScalar = 1 / numeric_cast<FloatingPointScalarT>(scalar);

            ONYX_ASSERT(IsMultiplicationSafe(X, invScalar));
            ONYX_ASSERT(IsMultiplicationSafe(Y, invScalar));
            ONYX_ASSERT(IsMultiplicationSafe(Z, invScalar));

            X = numeric_cast<ScalarT>(X * invScalar);
            Y = numeric_cast<ScalarT>(Y * invScalar);
            Z = numeric_cast<ScalarT>(Z * invScalar);
        }

        constexpr Vector3 operator+(const Vector3& rhs) const
        {
            ONYX_ASSERT(IsAdditionSafe(X, rhs.X));
            ONYX_ASSERT(IsAdditionSafe(Y, rhs.Y));
            ONYX_ASSERT(IsAdditionSafe(Z, rhs.Z));

            return Vector3(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
        }

        constexpr Vector3 operator-(const Vector3& rhs) const
        {
            ONYX_ASSERT(IsSubtractionSafe(X, rhs.X));
            ONYX_ASSERT(IsSubtractionSafe(Y, rhs.Y));
            ONYX_ASSERT(IsSubtractionSafe(Z, rhs.Z));

            return Vector3(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
        }

        constexpr Vector3 operator*(ScalarT scalar) const
        {
            //ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(Z, scalar));

            return Vector3(X * scalar, Y * scalar, Z * scalar);
        }

        constexpr Vector3 operator/(ScalarT scalar) const
        {
            ONYX_ASSERT(IsDivisionSafe(X, scalar));
            ONYX_ASSERT(IsDivisionSafe(Y, scalar));
            ONYX_ASSERT(IsDivisionSafe(Z, scalar));

            return Vector3(X / scalar, Y / scalar, Z / scalar);
        }

        constexpr bool operator==(const Vector3& rhs) const { return IsEqual(rhs); }
        constexpr bool operator!=(const Vector3& rhs) const { return IsEqual(rhs) == false; }

        template <typename T>
        constexpr VectorCompareMask operator<(const Vector3<T>& rhs) const
        {
            return ((X < rhs.X) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((Y < rhs.Y) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((Z < rhs.Z) ? VectorCompareMask::Z : VectorCompareMask::None);
        }

        template <typename T>
        constexpr VectorCompareMask operator<=(const Vector3<T>& rhs) const
        {
            return ((X <= rhs.X) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((Y <= rhs.Y) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((Z <= rhs.Z) ? VectorCompareMask::Z : VectorCompareMask::None);
        }


        template <typename T>
        constexpr VectorCompareMask operator>(const Vector3<T>& rhs) const
        {
            return((X > rhs.X) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((Y > rhs.Y) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((Z > rhs.Z) ? VectorCompareMask::Z : VectorCompareMask::None);
        }

        template <typename T>
        constexpr VectorCompareMask operator>=(const Vector3<T>& rhs) const
        {
            return((X >= rhs.X) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((Y >= rhs.Y) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((Z >= rhs.Z) ? VectorCompareMask::Z : VectorCompareMask::None);
        }

        constexpr bool IsEqual(const Vector3& rhs) const
        {
            return Onyx::IsEqual(X, rhs.X) &&
                Onyx::IsEqual(Y, rhs.Y) &&
                Onyx::IsEqual(Z, rhs.Z);
        }

        constexpr bool IsEqual(const Vector3& rhs, ScalarT epsilon) const requires std::is_floating_point_v<Scalar>
        {
            return Onyx::IsEqual(X, rhs.X, epsilon) &&
                Onyx::IsEqual(Y, rhs.Y, epsilon) &&
                Onyx::IsEqual(Z, rhs.Z, epsilon);
        }

        constexpr bool IsZero() const
        {
            return Onyx::IsZero(X) &&
                Onyx::IsZero(Y) &&
                Onyx::IsZero(Z);
        }

        constexpr bool IsZero(ScalarT epsilon) const requires std::is_floating_point_v<Scalar>
        {
            return Onyx::IsZero(X, epsilon) &&
                Onyx::IsZero(Y, epsilon) &&
                Onyx::IsZero(Z, epsilon);
        }

        static bool FromString(const StringView& str, Vector3& outVector)
        {
            constexpr StringView delimiter = " ";
            onyxS32 i = -1;
            for (const auto& split : std::views::split(str, delimiter))
            {
                std::from_chars(split.data(), split.data() + split.size(), outVector[++i]);
            }

            return (i == 3);
        }
    };

    template <typename Scalar>
    constexpr Bivector3<Scalar> Wedge(const Vector3<Scalar>& lhs, const Vector3<Scalar>& rhs)
    {
        return Bivector3<Scalar>
        {
                lhs[0] * rhs.Y - lhs[1] * rhs.X, // XY
                lhs[0] * rhs.Z - lhs[2] * rhs.X, // XZ
                lhs[1] * rhs.Z - lhs[2] * rhs.Y  // YZ
        };
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator+(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs.X));
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs.Y));
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs.Z));

        return Vector3<Scalar>(lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator-(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs.X));
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs.Y));
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs.Z));

        return Vector3<Scalar>(lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator*(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        //ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.X));
        //ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.Y));
        //ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.Z));

        return Vector3<Scalar>(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator/(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs.X));
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs.Y));
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs.Z));

        return Vector3<Scalar>(lhs / rhs.X, lhs / rhs.Y, lhs / rhs.Z);
    }

    template <typename Scalar>
    constexpr Bivector3<Scalar> Bivector3<Scalar>::XY_Unit()
    {
        return Vector3<Scalar>::X_Unit().Wedge(Vector3<Scalar>::Y_Unit());
    }

    template <typename Scalar>
    constexpr Bivector3<Scalar> Bivector3<Scalar>::ZX_Unit()
    {
        return Vector3<Scalar>::Z_Unit().Wedge(Vector3<Scalar>::X_Unit());
    }

    template <typename Scalar>
    constexpr Bivector3<Scalar> Bivector3<Scalar>::YZ_Unit()
    {
        return Vector3<Scalar>::Y_Unit().Wedge(Vector3<Scalar>::Z_Unit());
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> Abs(Vector3<Scalar> vec3)
    {
        return { std::abs(vec3[0]), std::abs(vec3[1]), std::abs(vec3[2]) };
    }

    // Vector2 constructor from Vector3
    template <typename Scalar>
    constexpr Vector2<Scalar>::Vector2(const Vector3<Scalar>& vec3)
        : m_Components{ vec3[0], vec3[1] }
    {
    }
}
