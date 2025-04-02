#pragma once

namespace Onyx
{
    template <typename Scalar>
    struct Matrix2
    {
    public:
        //constexpr Matrix3<Scalar> ZERO = Matrix3<Scalar>(0, 0, 0, 0, 0, 0, 0, 0, 0);
        //constexpr Matrix3<Scalar> IDENDITY = Matrix3<Scalar>(1, 0, 0, 0, 1, 0, 0, 0, 1);
        //constexpr Matrix3<Scalar> INVERSE = Matrix3<Scalar>(-1, -1, -1, -1, -1, -1, -1, -1, -1);

        Matrix2() = default;

        constexpr Matrix2(Scalar m00, Scalar m01, Scalar m10, Scalar m11)
            : m_Components{ m00, m01, m10, m11 }
        {
        }

        Scalar operator[](onyxU8 index) const { return m_Components[index]; }

    private:
        Scalar m_Components[4] = { 1, 0, 0, 1 }; // IDENTITY MATRIX
    };

    template<typename Scalar>
    Vector2<Scalar> operator*(const Vector2<Scalar>& lhs, const Matrix2<Scalar>& m)
    {
        return Vector2<Scalar>
        {
            lhs[0] * m[0] + lhs[1] * m[1],
            lhs[0] * m[2] + lhs[1] * m[3]
        };
    }
}