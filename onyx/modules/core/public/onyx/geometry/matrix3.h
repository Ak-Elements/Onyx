#pragma once

namespace Onyx
{
    template <typename Scalar>
    struct Matrix3
    {
    public:
        //constexpr Matrix3<Scalar> ZERO = Matrix3<Scalar>(0, 0, 0, 0, 0, 0, 0, 0, 0);
        //constexpr Matrix3<Scalar> IDENDITY = Matrix3<Scalar>(1, 0, 0, 0, 1, 0, 0, 0, 1);
        //constexpr Matrix3<Scalar> INVERSE = Matrix3<Scalar>(-1, -1, -1, -1, -1, -1, -1, -1, -1);

        Matrix3()
            :m_Columns
            {
                Vector3<Scalar>(1,0,0),
                Vector3<Scalar>(0,1,0),
                Vector3<Scalar>(0,0,1)
            }
        {
        }

        constexpr Matrix3(Scalar m00, Scalar m01, Scalar m02, Scalar m10, Scalar m11, Scalar m12, Scalar m20, Scalar m21, Scalar m22)
            : m_Columns
            {
                { m00, m01, m02 },
                { m10, m11, m12 },
                { m20, m21, m22 }
            }
        {
        }

        constexpr Matrix3(Vector3<Scalar> column0, Vector3<Scalar> column1, Vector3<Scalar> column2)
            : m_Columns { column0, column1, column2 }
        {
        }

        Matrix3<Scalar> operator*(const Matrix3& rhs) const
        {
            const Vector3<Scalar>& SrcA0 = m_Columns[0];
            const Vector3<Scalar>& SrcA1 = m_Columns[1];
            const Vector3<Scalar>& SrcA2 = m_Columns[2];

            const Vector3<Scalar>& SrcB0 = rhs[0];
            const Vector3<Scalar>& SrcB1 = rhs[1];
            const Vector3<Scalar>& SrcB2 = rhs[2];

            Matrix3<Scalar> Result;

            Vector3<Scalar> tmp;
            tmp = SrcA0 * SrcB0[0];
            tmp += SrcA1 * SrcB0[1];
            tmp += SrcA2 * SrcB0[2];
            Result[0] = tmp;
            tmp = SrcA0 * SrcB1[0];
            tmp += SrcA1 * SrcB1[1];
            tmp += SrcA2 * SrcB1[2];
            Result[1] = tmp;
            tmp = SrcA0 * SrcB2[0];
            tmp += SrcA1 * SrcB2[1];
            tmp += SrcA2 * SrcB2[2];
            Result[2] = tmp;

            return Result;
        }

        Vector3<Scalar>& operator[](onyxU8 columnIndex) { return m_Columns[columnIndex]; }
        const Vector3<Scalar>& operator[](onyxU8 columnIndex) const { return m_Columns[columnIndex]; }

        void Inverse()
        {
            // Calculate the determinant
            Scalar det = m_Columns[0][0] * (m_Columns[1][1] * m_Columns[2][2] - m_Columns[1][2] * m_Columns[2][1]) -
                m_Columns[0][1] * (m_Columns[1][0] * m_Columns[2][2] - m_Columns[1][2] * m_Columns[2][0]) +
                m_Columns[0][2] * (m_Columns[1][0] * m_Columns[2][1] - m_Columns[1][1] * m_Columns[2][0]);

            if (IsZero(det)) // Check for singular matrix
            {
                throw std::runtime_error("Matrix is not invertible (determinant is zero)");
            }

            Scalar invDet = static_cast<Scalar>(1) / det;

            // Calculate the cofactors and the adjugate matrix
            Matrix3<Scalar> adj;
            adj[0][0] = (m_Columns[1][1] * m_Columns[2][2] - m_Columns[1][2] * m_Columns[2][1]) * invDet;
            adj[0][1] = -(m_Columns[0][1] * m_Columns[2][2] - m_Columns[0][2] * m_Columns[2][1]) * invDet;
            adj[0][2] = (m_Columns[0][1] * m_Columns[1][2] - m_Columns[0][2] * m_Columns[1][1]) * invDet;

            adj[1][0] = -(m_Columns[1][0] * m_Columns[2][2] - m_Columns[1][2] * m_Columns[2][0]) * invDet;
            adj[1][1] = (m_Columns[0][0] * m_Columns[2][2] - m_Columns[0][2] * m_Columns[2][0]) * invDet;
            adj[1][2] = -(m_Columns[0][0] * m_Columns[1][2] - m_Columns[0][2] * m_Columns[1][0]) * invDet;

            adj[2][0] = (m_Columns[1][0] * m_Columns[2][1] - m_Columns[1][1] * m_Columns[2][0]) * invDet;
            adj[2][1] = -(m_Columns[0][0] * m_Columns[2][1] - m_Columns[0][1] * m_Columns[2][0]) * invDet;
            adj[2][2] = (m_Columns[0][0] * m_Columns[1][1] - m_Columns[0][1] * m_Columns[1][0]) * invDet;

            // Update the matrix with the inverse
            *this = adj;
        }

    private:
        Vector3<Scalar> m_Columns[3];
    };

    template<typename Scalar>
    Vector3<Scalar> operator*(const Matrix3<Scalar>& m, const Vector3<Scalar>& v)
    {
        return Vector3<Scalar>(
            v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0],
            v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1],
            v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2]);
    }

    template<typename Scalar>
    Vector3<Scalar> operator*(const Vector3<Scalar>& v, const Matrix3<Scalar>& m)
    {
        return Vector3<Scalar>(
            v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2],
            v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2],
            v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2]);
    }
}