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

        constexpr Vector3()
            : Vector3(0, 0, 0)
        {
        }

        explicit constexpr Vector3(ScalarT scalar)
            : m_Components{ scalar, scalar, scalar }
        {
        }

        constexpr Vector3(ScalarT x, ScalarT y, ScalarT z)
            : m_Components{ x, y, z }
        {
        }

        constexpr Vector3(const Vector2<ScalarT>& vec2, ScalarT z)
            : m_Components{ vec2[0], vec2[1], z }
        {
        }

        template <typename OtherScalarT>
        explicit constexpr Vector3(const Vector3<OtherScalarT> otherVec)
            : m_Components { numeric_cast<ScalarT>(otherVec[0]), numeric_cast<ScalarT>(otherVec[1]),  numeric_cast<ScalarT>(otherVec[2]) }
        {
        }

        constexpr explicit Vector3(const Vector4<ScalarT>& vec4);

        constexpr ScalarT& operator[] (onyxU32 index) { ONYX_ASSERT(index < 3, "Axis index out of bounds."); return m_Components[index]; }
        constexpr ScalarT operator[] (onyxU32 index) const { ONYX_ASSERT(index < 3, "Axis index out of bounds."); return m_Components[index]; }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr Vector3 operator-() const { return Vector3(-m_Components[0], -m_Components[1], -m_Components[2]); }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr void Inverse()
        {
            m_Components[0] = -m_Components[0];
            m_Components[1] = -m_Components[1];
            m_Components[2] = -m_Components[2];
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

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr void Normalize()
        {
            const onyxF64 length = Length();
            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");

            const FloatingPointScalarT invLength = numeric_cast<FloatingPointScalarT>(1 / length);
            m_Components[0] = numeric_cast<Scalar>(m_Components[0] * invLength);
            m_Components[1] = numeric_cast<Scalar>(m_Components[1] * invLength);
            m_Components[2] = numeric_cast<Scalar>(m_Components[2] * invLength);
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
                numeric_cast<FloatingPointScalarT>(m_Components[0] * invLength),
                numeric_cast<FloatingPointScalarT>(m_Components[1] * invLength),
                numeric_cast<FloatingPointScalarT>(m_Components[2] * invLength)
            };
        }

        constexpr DotT Dot(const Vector3& rhs) const
        {
            return numeric_cast<DotT>(m_Components[0]) * numeric_cast<DotT>(rhs[0]) +
                numeric_cast<DotT>(m_Components[1]) * numeric_cast<DotT>(rhs[1]) +
                numeric_cast<DotT>(m_Components[2]) * numeric_cast<DotT>(rhs[2]);
        }

        constexpr Vector3 Cross(const Vector3& rhs) const
        {
            return
            {
                m_Components[1] * rhs[2] - rhs[1] * m_Components[2],
                m_Components[2] * rhs[0] - rhs[2] * m_Components[0],
                m_Components[0] * rhs[1] - rhs[0] * m_Components[1]
            };
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr void Rotate(onyxF64 radians, const Bivector3<ScalarT>& bivector)
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
            q[0] = a * m_Components[0] + m_Components[1] * b01 + m_Components[2] * b02;
            q[1] = a * m_Components[1] - m_Components[0] * b01 + m_Components[2] * b12;
            q[2] = a * m_Components[2] - m_Components[0] * b02 - m_Components[1] * b12;

            ScalarT q012 = m_Components[0] * b12 - m_Components[1] * b02 + m_Components[2] * b01; // trivector

            // r = q P*
            m_Components[0] = a * q[0] + q[1] * b01 + q[2] * b02 + q012 * b12;
            m_Components[1] = a * q[1] - q[0] * b01 - q012 * b02 + q[2] * b12;
            m_Components[2] = a * q[2] + q012 * b01 - q[0] * b02 - q[1] * b12;

            // trivector part of the result is always zero!
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr auto RotatedAroundX(onyxF64 radians) const
        {
            return Rotated(radians, Bivector3<FloatingPointScalarT>::YZ_Unit());
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr auto RotatedAroundY(onyxF64 radians) const
        {
            return Rotated(radians, Bivector3<FloatingPointScalarT>::ZX_Unit());
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr auto RotatedAroundZ(onyxF64 radians) const
        {
            return Rotated(radians, Bivector3<FloatingPointScalarT>::XY_Unit());
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr Vector3<FloatingPointScalarT> Rotated(onyxF64 radians, const Bivector3<FloatingPointScalarT>& bivector) const
        {
            const FloatingPointScalarT halfRadians = numeric_cast<FloatingPointScalarT>(radians / 2.0);
            FloatingPointScalarT sina = std::sin(halfRadians);
            FloatingPointScalarT a = std::cos(halfRadians);

            // the left side of the products have b a, not a b
            FloatingPointScalarT b01 = -sina * bivector[0];
            FloatingPointScalarT b02 = -sina * bivector[1];
            FloatingPointScalarT b12 = -sina * bivector[2];

            // q = P x
            const FloatingPointScalarT q0 = a * m_Components[0] + m_Components[1] * b01 + m_Components[2] * b02;
            const FloatingPointScalarT q1 = a * m_Components[1] - m_Components[0] * b01 + m_Components[2] * b12;
            const FloatingPointScalarT q2 = a * m_Components[2] - m_Components[0] * b02 - m_Components[1] * b12;

            FloatingPointScalarT q012 = m_Components[0] * b12 - m_Components[1] * b02 + m_Components[2] * b01; // trivector

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

            const FloatingPointScalarT q0 = a * m_Components[0] + m_Components[1] * b01 + m_Components[2] * b02;
            const FloatingPointScalarT q1 = a * m_Components[1] - m_Components[0] * b01 + m_Components[2] * b12;
            const FloatingPointScalarT q2 = a * m_Components[2] - m_Components[0] * b02 - m_Components[1] * b12;

            FloatingPointScalarT q012 = m_Components[0] * b12 - m_Components[1] * b02 + m_Components[2] * b01; // trivector

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
                m_Components[0] * rhs[1] - m_Components[1] * rhs[0], // XY
                m_Components[0] * rhs[2] - m_Components[2] * rhs[0], // XZ
                m_Components[1] * rhs[2] - m_Components[2] * rhs[1]  // YZ
            };
        }

        constexpr DotT operator|(const Vector3& rhs) const
        {
            return Dot(rhs);
        }

        constexpr void operator+=(const Vector3& rhs)
        {
            //ONYX_ASSERT(IsAdditionSafe(m_Components[0], rhs[0]));
            //ONYX_ASSERT(IsAdditionSafe(m_Components[1], rhs[1]));
            //ONYX_ASSERT(IsAdditionSafe(m_Components[2], rhs[2]));

            m_Components[0] += rhs[0];
            m_Components[1] += rhs[1];
            m_Components[2] += rhs[2];
        }

        constexpr void operator-=(const Vector3& rhs)
        {
            ONYX_ASSERT(IsSubtractionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[1], rhs[1]));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[2], rhs[2]));

            m_Components[0] -= rhs[0];
            m_Components[1] -= rhs[1];
            m_Components[2] -= rhs[2];
        }

        constexpr void operator*=(const ScalarT scalar)
        {
            /*ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], scalar));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[2], scalar));*/

            m_Components[0] *= scalar;
            m_Components[1] *= scalar;
            m_Components[2] *= scalar;
        }

        constexpr void operator/=(const ScalarT scalar)
        {
            const FloatingPointScalarT invScalar = 1 / numeric_cast<FloatingPointScalarT>(scalar);

            ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], invScalar));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], invScalar));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[2], invScalar));

            m_Components[0] = numeric_cast<ScalarT>(m_Components[0] * invScalar);
            m_Components[1] = numeric_cast<ScalarT>(m_Components[1] * invScalar);
            m_Components[2] = numeric_cast<ScalarT>(m_Components[2] * invScalar);
        }

        constexpr Vector3 operator+(const Vector3& rhs) const
        {
            ONYX_ASSERT(IsAdditionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsAdditionSafe(m_Components[1], rhs[1]));
            ONYX_ASSERT(IsAdditionSafe(m_Components[2], rhs[2]));

            return Vector3(m_Components[0] + rhs[0], m_Components[1] + rhs[1], m_Components[2] + rhs[2]);
        }

        constexpr Vector3 operator-(const Vector3& rhs) const
        {
            ONYX_ASSERT(IsSubtractionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[1], rhs[1]));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[2], rhs[2]));

            return Vector3(m_Components[0] - rhs[0], m_Components[1] - rhs[1], m_Components[2] - rhs[2]);
        }

        constexpr Vector3 operator*(ScalarT scalar) const
        {
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[2], scalar));

            return Vector3(m_Components[0] * scalar, m_Components[1] * scalar, m_Components[2] * scalar);
        }

        constexpr Vector3 operator/(ScalarT scalar) const
        {
            ONYX_ASSERT(IsDivisionSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsDivisionSafe(m_Components[1], scalar));
            ONYX_ASSERT(IsDivisionSafe(m_Components[2], scalar));

            return Vector3(m_Components[0] / scalar, m_Components[1] / scalar, m_Components[2] / scalar);
        }

        constexpr bool operator==(const Vector3& rhs) const { return IsEqual(rhs); }
        constexpr bool operator!=(const Vector3& rhs) const { return IsEqual(rhs) == false; }

        template <typename T>
        constexpr VectorCompareMask operator<(const Vector3<T>& rhs) const
        {
            return ((m_Components[0] < rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] < rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((m_Components[2] < rhs[2]) ? VectorCompareMask::Z : VectorCompareMask::None);
        }

        template <typename T>
        constexpr VectorCompareMask operator<=(const Vector3<T>& rhs) const
        {
            return ((m_Components[0] <= rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] <= rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((m_Components[2] <= rhs[2]) ? VectorCompareMask::Z : VectorCompareMask::None);
        }


        template <typename T>
        constexpr VectorCompareMask operator>(const Vector3<T>& rhs) const
        {
            return((m_Components[0] > rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] > rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((m_Components[2] > rhs[2]) ? VectorCompareMask::Z : VectorCompareMask::None);
        }

        template <typename T>
        constexpr VectorCompareMask operator>=(const Vector3<T>& rhs) const
        {
            return((m_Components[0] >= rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] >= rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None) |
                ((m_Components[2] >= rhs[2]) ? VectorCompareMask::Z : VectorCompareMask::None);
        }

        constexpr bool IsEqual(const Vector3& rhs) const
        {
            return Onyx::IsEqual(m_Components[0], rhs[0]) &&
                Onyx::IsEqual(m_Components[1], rhs[1]) &&
                Onyx::IsEqual(m_Components[2], rhs[2]);
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr bool IsEqual(const Vector3& rhs, ScalarT epsilon) const
        {
            return Onyx::IsEqual(m_Components[0], rhs[0], epsilon) &&
                Onyx::IsEqual(m_Components[1], rhs[1], epsilon) &&
                Onyx::IsEqual(m_Components[2], rhs[2], epsilon);
        }

        constexpr bool IsZero() const
        {
            return Onyx::IsZero(m_Components[0]) &&
                Onyx::IsZero(m_Components[1]) &&
                Onyx::IsZero(m_Components[2]);
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr bool IsZero(ScalarT epsilon) const
        {
            return Onyx::IsZero(m_Components[0], epsilon) &&
                Onyx::IsZero(m_Components[1], epsilon) &&
                Onyx::IsZero(m_Components[2], epsilon);
        }

        static bool FromString(const StringView& str, Vector3& outVector)
        {
            constexpr StringView delimiter = " ";
            onyxS32 i = -1;
            for (const auto& split : std::views::split(str, delimiter))
            {
                std::from_chars(split.data(), split.data() + split.size(), outVector.m_Components[++i]);
            }

            return (i == 3);
        }

    private:
        ScalarT m_Components[3];
    };

    template <typename Scalar>
    constexpr Bivector3<Scalar> Wedge(const Vector3<Scalar>& lhs, const Vector3<Scalar>& rhs)
    {
        return Bivector3<Scalar>
        {
                lhs[0] * rhs[1] - lhs[1] * rhs[0], // XY
                lhs[0] * rhs[2] - lhs[2] * rhs[0], // XZ
                lhs[1] * rhs[2] - lhs[2] * rhs[1]  // YZ
        };
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator+(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs[1]));
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs[2]));

        return Vector3<Scalar>(lhs + rhs[0], lhs + rhs[1], lhs + rhs[2]);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator-(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs[1]));
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs[2]));

        return Vector3<Scalar>(lhs - rhs[0], lhs - rhs[1], lhs - rhs[2]);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator*(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        //ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs[0]));
        //ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs[1]));
        //ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs[2]));

        return Vector3<Scalar>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar> operator/(Scalar lhs, const Vector3<Scalar>& rhs)
    {
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs[1]));
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs[2]));

        return Vector3<Scalar>(lhs / rhs[0], lhs / rhs[1], lhs / rhs[2]);
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
