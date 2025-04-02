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
            : m_Components{ ScalarT{}, ScalarT{} }
        {
        }

        constexpr explicit Vector2(ScalarT scalar)
            : m_Components { scalar, scalar }
        {
        }

        constexpr Vector2(ScalarT x, ScalarT y)
            : m_Components { x, y }
        {
        }

        template <typename OtherScalarT>
        constexpr explicit Vector2(const Vector2<OtherScalarT>& vec2)
            : m_Components{ numeric_cast<ScalarT>(vec2[0]), numeric_cast<ScalarT>(vec2[1]) }
        {
            
        }
        constexpr explicit Vector2(const Vector3<ScalarT>& vec3);

        constexpr ScalarT& operator[] (onyxU32 index) { ONYX_ASSERT(index < 2, "Axis index out of bounds."); return m_Components[index]; }
        constexpr ScalarT operator[] (onyxU32 index) const { ONYX_ASSERT(index < 2, "Axis index out of bounds."); return m_Components[index]; }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr Vector2 operator-() const { return Vector2(-m_Components[0], -m_Components[1]); }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr void Inverse()
        {
            m_Components[0] = -m_Components[0];
            m_Components[1] = -m_Components[1];
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
            m_Components[0] = x;
            m_Components[1] = y;
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr void Normalize()
        {
            const FloatingPointScalarT length = Length();

            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");

            const FloatingPointScalarT invLength = (1 / length);
            m_Components[0] *= invLength;
            m_Components[1] *= invLength;
        }

        constexpr Vector2<FloatingPointScalarT> Normalized() const
        {
            const FloatingPointScalarT length = Length();
            
            ONYX_ASSERT(Onyx::IsZero(length) == false, "Can not normalize a vector of length 0");
            
            const FloatingPointScalarT invLength = (1 / length);
            return Vector2<FloatingPointScalarT>(m_Components[0] * invLength, m_Components[1] * invLength);
        }

        constexpr DotT Dot(const Vector2& rhs) const
        {
            return numeric_cast<DotT>(m_Components[0]) * numeric_cast<DotT>(rhs[0]) +
                numeric_cast<DotT>(m_Components[1]) * numeric_cast<DotT>(rhs[1]);
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
            return (static_cast<OutT>(m_Components[0]) * static_cast<OutT>(rhs[1])) -
                (static_cast<OutT>(m_Components[1]) * static_cast<OutT>(rhs[0]));
        }

        constexpr DotT operator|(const Vector2& rhs) const
        {
            return Dot(rhs);
        }

        constexpr void operator+=(const ScalarT scalar)
        {
            ONYX_ASSERT(IsAdditionSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsAdditionSafe(m_Components[1], scalar));

            m_Components[0] += scalar;
            m_Components[1] += scalar;
        }

        constexpr void operator+=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsAdditionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsAdditionSafe(m_Components[1], rhs[1]));

            m_Components[0] += rhs[0];
            m_Components[1] += rhs[1];
        }

        constexpr void operator-=(const ScalarT scalar)
        {
            ONYX_ASSERT(IsSubtractionSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[1], scalar));

            m_Components[0] -= scalar;
            m_Components[1] -= scalar;
        }

        constexpr void operator-=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsSubtractionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[1], rhs[1]));

            m_Components[0] -= rhs[0];
            m_Components[1] -= rhs[1];
        }

        constexpr void operator*=(const ScalarT scalar)
        {
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], scalar));

            m_Components[0] *= scalar;
            m_Components[1] *= scalar;
        }

        constexpr void operator*=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], rhs[1]));

            m_Components[0] *= rhs[0];
            m_Components[1] *= rhs[1];
        }

        constexpr void operator/=(const ScalarT scalar)
        {
            const FloatingPointScalarT invScalar = 1 / numeric_cast<FloatingPointScalarT>(scalar);

            ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], invScalar));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], invScalar));

            m_Components[0] = numeric_cast<ScalarT>(m_Components[0] * invScalar);
            m_Components[1] = numeric_cast<ScalarT>(m_Components[1] * invScalar);
        }

        constexpr void operator/=(const Vector2& rhs)
        {
            ONYX_ASSERT(IsDivisionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsDivisionSafe(m_Components[1], rhs[1]));

            m_Components[0] /= rhs[0];
            m_Components[1] /= rhs[1];
        }

        constexpr Vector2 operator+(ScalarT scalar) const
        {
            ONYX_ASSERT(IsAdditionSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsAdditionSafe(m_Components[1], scalar));

            return Vector2(m_Components[0] + scalar, m_Components[1] + scalar);
        }

        constexpr Vector2 operator+(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsAdditionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsAdditionSafe(m_Components[1], rhs[1]));

            return Vector2(m_Components[0] + rhs[0], m_Components[1] + rhs[1]);
        }

        constexpr Vector2 operator-(ScalarT scalar) const
        {
            ONYX_ASSERT(IsSubtractionSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[1], scalar));

            return Vector2(m_Components[0] - scalar, m_Components[1] - scalar);
        }

        constexpr Vector2 operator-(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsSubtractionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsSubtractionSafe(m_Components[1], rhs[1]));

            return Vector2(m_Components[0] - rhs[0], m_Components[1] - rhs[1]);
        }

        constexpr Vector2 operator*(ScalarT scalar) const
        {
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], scalar));
            //ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], scalar));

            return Vector2(m_Components[0] * scalar, m_Components[1] * scalar);
        }

        constexpr Vector2 operator*(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsMultiplicationSafe(m_Components[1], rhs[1]));

            return Vector2(m_Components[0] * rhs[0], m_Components[1] * rhs[1]);
        }

        constexpr Vector2 operator/(ScalarT scalar) const
        {
            ONYX_ASSERT(IsDivisionSafe(m_Components[0], scalar));
            ONYX_ASSERT(IsDivisionSafe(m_Components[1], scalar));

            return Vector2(m_Components[0] / scalar, m_Components[1] / scalar);
        }

        constexpr Vector2 operator/(const Vector2& rhs) const
        {
            ONYX_ASSERT(IsDivisionSafe(m_Components[0], rhs[0]));
            ONYX_ASSERT(IsDivisionSafe(m_Components[0], rhs[1]));

            return Vector2(m_Components[0] / rhs[0], m_Components[1] / rhs[1]);
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr void Rotate(ScalarT radians)
        {
            Scalar cosAlpha = std::cos(radians);
            Scalar sinAlpha = std::sin(radians);

            Scalar rotatedX = cosAlpha * m_Components[0] - sinAlpha * m_Components[1];
            Scalar rotatedY = sinAlpha * m_Components[0] + cosAlpha * m_Components[1];
            m_Components[0] = rotatedX;
            m_Components[1] = rotatedY;
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT> || std::is_signed_v<ScalarT>>>
        constexpr auto Rotated(onyxF64 radians) const
        {
            FloatingPointScalarT rad = numeric_cast<FloatingPointScalarT>(radians);
            FloatingPointScalarT cosAlpha = std::cos(rad);
            FloatingPointScalarT sinAlpha = std::sin(rad);

            return Vector2<FloatingPointScalarT>
            {
                numeric_cast<FloatingPointScalarT>(cosAlpha * m_Components[0] - sinAlpha * m_Components[1]),
                numeric_cast<FloatingPointScalarT>(sinAlpha * m_Components[0] + cosAlpha * m_Components[1])
            };
        }

        constexpr auto RotatedTo(Vector2 to) const
        {
            const Vector2<FloatingPointScalarT>& fromNormalized = Normalized();
            const Vector2<FloatingPointScalarT>& toNormalized = to.Normalized();

            FloatingPointScalarT cosAlpha = numeric_cast<FloatingPointScalarT>(toNormalized.Dot(fromNormalized));
            FloatingPointScalarT sinAlpha = fromNormalized[0] * toNormalized[1] - fromNormalized[1] * toNormalized[0];

            return Vector2<FloatingPointScalarT>
            {
                numeric_cast<FloatingPointScalarT>(cosAlpha* m_Components[0] - sinAlpha * m_Components[1]),
                numeric_cast<FloatingPointScalarT>(sinAlpha* m_Components[0] + cosAlpha * m_Components[1])
            };
        }

        constexpr bool operator==(const Vector2& rhs) const { return IsEqual(rhs); }
        constexpr bool operator!=(const Vector2& rhs) const { return IsEqual(rhs) == false; }

        template <typename T>
        constexpr VectorCompareMask operator<(const Vector2<T>& rhs) const
        {
            return ((m_Components[0] < rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                    ((m_Components[1] < rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None);
        }

        template <typename T>
        constexpr VectorCompareMask operator<=(const Vector2<T>& rhs) const
        {
            return ((m_Components[0] <= rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] <= rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None);
        }


        template <typename T>
        constexpr VectorCompareMask operator>(const Vector2<T>& rhs) const
        {
            return((m_Components[0] > rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] > rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None);
        }

        template <typename T>
        constexpr VectorCompareMask operator>=(const Vector2<T>& rhs) const
        {
            return((m_Components[0] >= rhs[0]) ? VectorCompareMask::X : VectorCompareMask::None) |
                ((m_Components[1] >= rhs[1]) ? VectorCompareMask::Y : VectorCompareMask::None);
        }

        constexpr bool IsEqual(const Vector2& rhs) const
        {
            return (Onyx::IsEqual(m_Components[0], rhs[0])) &&
                (Onyx::IsEqual(m_Components[1], rhs[1]));
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<Scalar>>>
        constexpr bool IsEqual(const Vector2& rhs, Scalar epsilon) const
        {
            return (Onyx::IsEqual(m_Components[0], rhs[0], epsilon)) &&
                (Onyx::IsEqual(m_Components[1], rhs[1], epsilon));
        }

        constexpr bool IsZero() const
        {
            return Onyx::IsZero(m_Components[0]) && Onyx::IsZero(m_Components[1]);
        }

        template <typename = std::enable_if_t<std::is_floating_point_v<ScalarT>>>
        constexpr bool IsZero(ScalarT epsilon) const
        {
            return Onyx::IsZero(m_Components[0], epsilon) && Onyx::IsZero(m_Components[1], epsilon);
        }

        constexpr Vector2 GetPerpendicularClockwise() const
        {
            return Vector2(m_Components[1], -m_Components[0]);
        }

        constexpr Vector2 GetPerpendicularCounterClockwise() const
        {
            return Vector2(-m_Components[1], m_Components[0]);
        }

	    template <typename OtherVectorT>
        constexpr void CopyTo(OtherVectorT& otherVec) const
        {
            otherVec[0] = m_Components[0];
            otherVec[1] = m_Components[1];
        }

        static bool FromString(const StringView& str, Vector2& outVector)
        {
            constexpr StringView delimiter = " ";
            onyxS32 i = -1;
            for (const auto& split : std::views::split(str, delimiter))
            {
                std::from_chars(split.data(), split.data() + split.size(), outVector.m_Components[++i]);
            }

            return (i == 2);
        }

    private:
        ScalarT m_Components[2];
    };

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator+(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsAdditionSafe(lhs, rhs[1]));

        return Vector2<ScalarT>(lhs + rhs[0], lhs + rhs[1], lhs + rhs[2]);
    }

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator-(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsSubtractionSafe(lhs, rhs[1]));

        return Vector2<ScalarT>(lhs - rhs[0], lhs - rhs[1], lhs - rhs[2]);
    }

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator*(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs[1]));

        return Vector2<ScalarT>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
    }

    template <typename ScalarT>
    constexpr Vector2<ScalarT> operator/(ScalarT lhs, const Vector2<ScalarT>& rhs)
    {
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs[0]));
        ONYX_ASSERT(IsDivisionSafe(lhs, rhs[1]));

        return Vector2<ScalarT>(lhs / rhs[0], lhs / rhs[1], lhs / rhs[2]);
    }
}