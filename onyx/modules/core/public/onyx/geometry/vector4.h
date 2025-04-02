#pragma once

#include <charconv>

namespace Onyx
{
    template <typename Scalar>
    struct Vector4
    {
    public:
        using ScalarT = Scalar;

        static constexpr Vector4 Zero() { return Vector4(0, 0, 0, 0); }
        static constexpr Vector4 X_Unit() { return Vector4(1, 0, 0, 0); }
        static constexpr Vector4 Y_Unit() { return Vector4(0, 1, 0, 0); }
        static constexpr Vector4 Z_Unit() { return Vector4(0, 0, 1, 0); }
        static constexpr Vector4 W_Unit() { return Vector4(0, 0, 0, 1); }

        constexpr Vector4() = default;

        explicit constexpr Vector4(Scalar scalar)
            : m_Components { scalar, scalar, scalar, scalar }
        {
        }

        constexpr Vector4(Scalar x, Scalar y, Scalar z, Scalar w)
            : m_Components { x, y, z, w }
        {
        }

        explicit constexpr Vector4(const Vector2<Scalar>& vec2, Scalar z = 0.0f, Scalar w = 0.0f)
            : m_Components{ vec2[0], vec2[1], z, w }
        {
        }

        explicit constexpr Vector4(const Vector3<Scalar>& vec3, Scalar w = 0.0f)
            : m_Components { vec3[0], vec3[1], vec3[2], w }
        {
        }

        constexpr Scalar& operator[] (onyxU32 index) { ONYX_ASSERT(index < 4, "Axis index out of bounds."); return m_Components[index]; }
        constexpr const Scalar& operator[] (onyxU32 index) const { ONYX_ASSERT(index < 4, "Axis index out of bounds."); return m_Components[index]; }

        constexpr Scalar Length() const
        {
            using std::sqrt;
            return sqrt(LengthSquared());
        }

        constexpr Scalar LengthSquared() const
        {
            return (m_Components[0] * m_Components[0]) + (m_Components[1] * m_Components[1]) + (m_Components[2] * m_Components[2]) + (m_Components[3] * m_Components[3]);
        }

        constexpr void Normalize()
        {
            //TODO assert
            const onyxF32 length = Length();

            if (length > std::numeric_limits<Scalar>::epsilon())
            {
                const onyxF32 invLength = (1 / length);
                m_Components[0] *= invLength;
                m_Components[1] *= invLength;
                m_Components[2] *= invLength;
                m_Components[3] *= invLength;
            }
        }

        constexpr Vector4 Normalized() const
        {
            static_assert(this != Vector4::Zero());

            const onyxF32 length = Length();
            if (length > std::numeric_limits<Scalar>::epsilon())
            {
                const onyxF32 invLength = (1 / length);
                return Vector4(m_Components[0] * invLength, m_Components[1] * invLength, m_Components[2] * invLength, m_Components[3] * invLength);
            }

            return Vector4();
        }

        constexpr Scalar Dot3D(const Vector4& rhs) const
        {
            return m_Components[0] * rhs.m_Components[0] + m_Components[1] * rhs.m_Components[1] + m_Components[2] * rhs.m_Components[2];
        }

        constexpr void operator+=(const Scalar scalar)
        {
            m_Components[0] += scalar;
            m_Components[1] += scalar;
            m_Components[2] += scalar;
            m_Components[3] += scalar;
        }

        constexpr void operator+=(const Vector4& rhs)
        {
            m_Components[0] += rhs.m_Components[0];
            m_Components[1] += rhs.m_Components[1];
            m_Components[2] += rhs.m_Components[2];
            m_Components[3] += rhs.m_Components[3];
        }

        constexpr void operator-=(const Scalar scalar)
        {
            m_Components[0] -= scalar;
            m_Components[1] -= scalar;
            m_Components[2] -= scalar;
            m_Components[3] -= scalar;
        }

        constexpr void operator-=(const Vector4& rhs)
        {
            m_Components[0] -= rhs.m_Components[0];
            m_Components[1] -= rhs.m_Components[1];
            m_Components[2] -= rhs.m_Components[2];
            m_Components[3] -= rhs.m_Components[3];
        }

        constexpr void operator*=(const Scalar scalar)
        {
            m_Components[0] *= scalar;
            m_Components[1] *= scalar;
            m_Components[2] *= scalar;
            m_Components[3] *= scalar;
        }

        constexpr void operator*=(const Vector4& rhs)
        {
            m_Components[0] *= rhs.m_Components[0];
            m_Components[1] *= rhs.m_Components[1];
            m_Components[2] *= rhs.m_Components[2];
            m_Components[3] *= rhs.m_Components[3];
        }

        constexpr void operator/=(const Scalar scalar)
        {
            const Scalar invScalar = 1 / scalar;
            m_Components[0] *= invScalar;
            m_Components[1] *= invScalar;
            m_Components[2] *= invScalar;
            m_Components[3] *= invScalar;
        }

        constexpr void operator/=(const Vector4& rhs)
        {
            m_Components[0] /= rhs.m_Components[0];
            m_Components[1] /= rhs.m_Components[1];
            m_Components[2] /= rhs.m_Components[2];
            m_Components[3] /= rhs.m_Components[3];
        }

        constexpr Vector4 operator+(Scalar scalar) const
        {
            return Vector4(m_Components[0] + scalar, m_Components[1] + scalar, m_Components[2] + scalar, m_Components[3] + scalar);
        }

        constexpr Vector4 operator+(const Vector4& rhs) const
        {
            return Vector4(m_Components[0] + rhs.m_Components[0], m_Components[1] + rhs.m_Components[1], m_Components[2] + rhs.m_Components[2], m_Components[3] + rhs.m_Components[3]);
        }

        constexpr Vector4 operator-(Scalar scalar) const
        {
            return Vector4(m_Components[0] - scalar, m_Components[1] - scalar, m_Components[2] - scalar, m_Components[3] - scalar);
        }

        Vector4 operator-(const Vector4& rhs) const
        {
            return Vector4(m_Components[0] - rhs.m_Components[0], m_Components[1] - rhs.m_Components[1], m_Components[2] - rhs.m_Components[2], m_Components[3] - rhs.m_Components[3]);
        }

        Vector4 operator*(Scalar scalar) const
        {
            return Vector4(m_Components[0] * scalar, m_Components[1] * scalar, m_Components[2] * scalar, m_Components[3] * scalar);
        }

        Vector4 operator*(const Vector4& rhs) const
        {
            return Vector4(m_Components[0] * rhs.m_Components[0], m_Components[1] * rhs.m_Components[1], m_Components[2] * rhs.m_Components[2], m_Components[3] * rhs.m_Components[3]);
        }

        Vector4 operator/(Scalar scalar) const
        {
            const Scalar invScalar = 1 / scalar;
            return Vector4(m_Components[0] * invScalar, m_Components[1] * invScalar, m_Components[2] * invScalar, m_Components[3] * invScalar);
        }

        constexpr Vector4 operator/(const Vector4& rhs) const
        {
            return Vector4(m_Components[0] / rhs.m_Components[0], m_Components[1] / rhs.m_Components[1], m_Components[2] / rhs.m_Components[2], m_Components[3] / rhs.m_Components[3]);
        }

        constexpr bool operator==(const Vector4& rhs) const { return IsApprox(rhs); }
        constexpr bool operator!=(const Vector4& rhs) const { return IsApprox(rhs) == false; }

        constexpr bool IsApprox(const Vector4& rhs) const
        {
            using std::abs;
            constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();
            return (abs(m_Components[0] - rhs.m_Components[0]) <= epsilon) &&
                (abs(m_Components[1] - rhs.m_Components[1]) <= epsilon) &&
                (abs(m_Components[2] - rhs.m_Components[2]) <= epsilon) &&
                (abs(m_Components[3] - rhs.m_Components[3]) <= epsilon);
        }

        constexpr bool IsApprox(const Vector4& rhs, Scalar epsilon) const
        {
            using std::abs;
            return (abs(m_Components[0] - rhs.m_Components[0]) <= epsilon) &&
                (abs(m_Components[1] - rhs.m_Components[1]) <= epsilon) &&
                (abs(m_Components[2] - rhs.m_Components[2]) <= epsilon) &&
                (abs(m_Components[3] - rhs.m_Components[3]) <= epsilon);
        }

        constexpr bool IsZero() const
        {
            using std::abs;
            constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();
            return (abs(m_Components[0]) <= epsilon) &&
                (abs(m_Components[1]) <= epsilon) &&
                (abs(m_Components[2]) <= epsilon) &&
                (abs(m_Components[3]) <= epsilon);
        }

        constexpr bool IsZero(Scalar epsilon) const
        {
            using std::abs;
            return (abs(m_Components[0]) <= epsilon) &&
                (abs(m_Components[1]) <= epsilon) &&
                (abs(m_Components[2]) <= epsilon) &&
                (abs(m_Components[3]) <= epsilon);
        }

        static Vector4 FromString(const StringView& str)
        {
            Vector4 result;
            constexpr StringView delimiter = " ";
            onyxS32 i = -1;
            for (const auto& split : std::views::split(str, delimiter))
            {
                std::from_chars(split.data(), split.data() + split.size(), result.m_Components[++i]);
            }

            ONYX_ASSERT(i == 3);
            return result;
        }

    private:
        Scalar m_Components[4];
    };

    template <typename Scalar>
    Vector4<Scalar> operator+(Scalar lhs, const Vector4<Scalar>& rhs)
    {
        return Vector4<Scalar>(lhs + rhs[0], lhs + rhs[1], lhs + rhs[2], lhs + rhs[3]);
    }

    template <typename Scalar>
    Vector4<Scalar> operator-(Scalar lhs, const Vector4<Scalar>& rhs)
    {
        return Vector4<Scalar>(lhs - rhs[0], lhs - rhs[1], lhs - rhs[2], lhs - rhs[3]);
    }

    template <typename Scalar>
    Vector4<Scalar> operator*(Scalar lhs, const Vector4<Scalar>& rhs)
    {
        return Vector4<Scalar>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
    }

    template <typename Scalar>
    Vector4<Scalar> operator/(Scalar lhs, const Vector4<Scalar>& rhs)
    {
        return Vector4<Scalar>(lhs / rhs[0], lhs / rhs[1], lhs / rhs[2], lhs / rhs[3]);
    }

    template <typename Scalar>
    constexpr Vector3<Scalar>::Vector3(const Vector4<Scalar>& vec4)
        : m_Components{ vec4[0], vec4[1], vec4[2] }
    {
    }
}
