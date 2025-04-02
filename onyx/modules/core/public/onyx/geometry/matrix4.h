#pragma once

#include <onyx/geometry/rotor3.h>

namespace Onyx
{
	namespace detail
	{
		/// Make a linear combination of two vectors and return the result.
		// result = (a * ascl) + (b * bscl)
		template<typename Scalar>
		Vector3<Scalar> combine(
			const Vector3<Scalar>& a,
			const Vector3<Scalar>& b,
			Scalar ascl, Scalar bscl)
		{
			return (a * ascl) + (b * bscl);
		}

		template<typename Scalar>
		Vector3<Scalar> scale(const Vector3<Scalar>& v, Scalar desiredLength)
		{
			return v * desiredLength / static_cast<Scalar>(v.Length());
		}
	}//namespace detail

    template <typename Scalar>
    struct ONYX_ALIGN(16) Matrix4
    {
    public:
        Matrix4()
            :m_Columns
		    {
			    Vector4<Scalar>(1,0,0,0),
			    Vector4<Scalar>(0,1,0,0),
			    Vector4<Scalar>(0,0,1,0),
			    Vector4<Scalar>(0,0,0,1),
		    }
        {
            
        }

		explicit constexpr Matrix4(Scalar identityScalar)
			: m_Columns
		    {
				Vector4<Scalar>(identityScalar,0,0,0),
				Vector4<Scalar>(0,identityScalar,0,0),
				Vector4<Scalar>(0,0,identityScalar,0),
				Vector4<Scalar>(0,0,0,identityScalar),
		    }
		{
		}

        constexpr Matrix4(Scalar m00, Scalar m01, Scalar m02, Scalar m03, Scalar m10, Scalar m11, Scalar m12, Scalar m13, Scalar m20, Scalar m21, Scalar m22, Scalar m23, Scalar m30, Scalar m31, Scalar m32, Scalar m33)
            : m_Columns
            {
				{ m00, m01, m02, m03 },
				{ m10, m11, m12, m13 },
				{ m20, m21, m22, m23 },
				{ m30, m31, m32, m33 }
            }
        {
        }

		constexpr Matrix4(const Vector4<Scalar>& column0, const Vector4<Scalar>& column1, const Vector4<Scalar>& column2, const Vector4<Scalar>& column3)
			: m_Columns
			{
				column0, column1, column2, column3
			}
		{
		}

		explicit constexpr Matrix4(Matrix3<Scalar> mat3)
			: m_Columns
			{
				Vector4<Scalar>(mat3[0], 0),
				Vector4<Scalar>(mat3[1], 0),
				Vector4<Scalar>(mat3[2], 0),
				Vector4<Scalar>(0,0,0,1),
			}
		{

		}

		Vector4<Scalar>& operator[](onyxU8 columnIndex) { return m_Columns[columnIndex]; }
		const Vector4<Scalar>& operator[](onyxU8 columnIndex) const { return m_Columns[columnIndex]; }

        //Scalar& operator[](onyxU8 columnIndex, onyxU8 rowIndex) { return m_Columns[columnIndex][rowIndex]; }
        //Scalar operator[](onyxU8 columnIndex, onyxU8 rowIndex) const { return m_Columns[columnIndex][rowIndex]; }

        Matrix4<Scalar> operator*(const Matrix4& rhs) const
        {
			const Vector4<Scalar>& SrcA0 = m_Columns[0];
			const Vector4<Scalar>& SrcA1 = m_Columns[1];
			const Vector4<Scalar>& SrcA2 = m_Columns[2];
			const Vector4<Scalar>& SrcA3 = m_Columns[3];

			const Vector4<Scalar>& SrcB0 = rhs[0];
			const Vector4<Scalar>& SrcB1 = rhs[1];
			const Vector4<Scalar>& SrcB2 = rhs[2];
			const Vector4<Scalar>& SrcB3 = rhs[3];

			Matrix4<Scalar> Result;

			Vector4<Scalar> tmp;
			tmp = SrcA0 * SrcB0[0];
			tmp += SrcA1 * SrcB0[1];
			tmp += SrcA2 * SrcB0[2];
			tmp += SrcA3 * SrcB0[3];
			Result[0] = tmp;
			tmp = SrcA0 * SrcB1[0];
			tmp += SrcA1 * SrcB1[1];
			tmp += SrcA2 * SrcB1[2];
			tmp += SrcA3 * SrcB1[3];
			Result[1] = tmp;
			tmp = SrcA0 * SrcB2[0];
			tmp += SrcA1 * SrcB2[1];
			tmp += SrcA2 * SrcB2[2];
			tmp += SrcA3 * SrcB2[3];
			Result[2] = tmp;
			tmp = SrcA0 * SrcB3[0];
			tmp += SrcA1 * SrcB3[1];
			tmp += SrcA2 * SrcB3[2];
			tmp += SrcA3 * SrcB3[3];
			Result[3] = tmp;

			return Result;
        }

		Matrix4<Scalar> operator*(Scalar scalar) const
		{
			return Matrix4<Scalar>(
				m_Columns[0] * scalar,
				m_Columns[1] * scalar,
				m_Columns[2] * scalar,
				m_Columns[3] * scalar);
		}

        Matrix4<Scalar> Inverse() const
        {
			Scalar Coef00 = m_Columns[2][2] * m_Columns[3][3] - m_Columns[3][2] * m_Columns[2][3];
			Scalar Coef02 = m_Columns[1][2] * m_Columns[3][3] - m_Columns[3][2] * m_Columns[1][3];
			Scalar Coef03 = m_Columns[1][2] * m_Columns[2][3] - m_Columns[2][2] * m_Columns[1][3];

			Scalar Coef04 = m_Columns[2][1] * m_Columns[3][3] - m_Columns[3][1] * m_Columns[2][3];
			Scalar Coef06 = m_Columns[1][1] * m_Columns[3][3] - m_Columns[3][1] * m_Columns[1][3];
			Scalar Coef07 = m_Columns[1][1] * m_Columns[2][3] - m_Columns[2][1] * m_Columns[1][3];

			Scalar Coef08 = m_Columns[2][1] * m_Columns[3][2] - m_Columns[3][1] * m_Columns[2][2];
			Scalar Coef10 = m_Columns[1][1] * m_Columns[3][2] - m_Columns[3][1] * m_Columns[1][2];
			Scalar Coef11 = m_Columns[1][1] * m_Columns[2][2] - m_Columns[2][1] * m_Columns[1][2];

			Scalar Coef12 = m_Columns[2][0] * m_Columns[3][3] - m_Columns[3][0] * m_Columns[2][3];
			Scalar Coef14 = m_Columns[1][0] * m_Columns[3][3] - m_Columns[3][0] * m_Columns[1][3];
			Scalar Coef15 = m_Columns[1][0] * m_Columns[2][3] - m_Columns[2][0] * m_Columns[1][3];

			Scalar Coef16 = m_Columns[2][0] * m_Columns[3][2] - m_Columns[3][0] * m_Columns[2][2];
			Scalar Coef18 = m_Columns[1][0] * m_Columns[3][2] - m_Columns[3][0] * m_Columns[1][2];
			Scalar Coef19 = m_Columns[1][0] * m_Columns[2][2] - m_Columns[2][0] * m_Columns[1][2];

			Scalar Coef20 = m_Columns[2][0] * m_Columns[3][1] - m_Columns[3][0] * m_Columns[2][1];
			Scalar Coef22 = m_Columns[1][0] * m_Columns[3][1] - m_Columns[3][0] * m_Columns[1][1];
			Scalar Coef23 = m_Columns[1][0] * m_Columns[2][1] - m_Columns[2][0] * m_Columns[1][1];

			Vector4<Scalar> Fac0(Coef00, Coef00, Coef02, Coef03);
			Vector4<Scalar> Fac1(Coef04, Coef04, Coef06, Coef07);
			Vector4<Scalar> Fac2(Coef08, Coef08, Coef10, Coef11);
			Vector4<Scalar> Fac3(Coef12, Coef12, Coef14, Coef15);
			Vector4<Scalar> Fac4(Coef16, Coef16, Coef18, Coef19);
			Vector4<Scalar> Fac5(Coef20, Coef20, Coef22, Coef23);

			Vector4<Scalar> Vec0(m_Columns[1][0], m_Columns[0][0], m_Columns[0][0], m_Columns[0][0]);
			Vector4<Scalar> Vec1(m_Columns[1][1], m_Columns[0][1], m_Columns[0][1], m_Columns[0][1]);
			Vector4<Scalar> Vec2(m_Columns[1][2], m_Columns[0][2], m_Columns[0][2], m_Columns[0][2]);
			Vector4<Scalar> Vec3(m_Columns[1][3], m_Columns[0][3], m_Columns[0][3], m_Columns[0][3]);

			Vector4<Scalar> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			Vector4<Scalar> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			Vector4<Scalar> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			Vector4<Scalar> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			Vector4<Scalar> SignA(+1, -1, +1, -1);
			Vector4<Scalar> SignB(-1, +1, -1, +1);
			Matrix4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

			Vector4<Scalar> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

			Vector4<Scalar> Dot0(m_Columns[0] * Row0);
			Scalar Dot1 = (Dot0[0] + Dot0[1]) + (Dot0[2] + Dot0[3]);

			Scalar OneOverDeterminant = static_cast<Scalar>(1) / Dot1;

			return Inverse * OneOverDeterminant;
        }

		bool Decompose(Vector3<Scalar>& outTranslation, Rotor3<Scalar>& outRotation, Vector3<Scalar>& outScale)
        {
			Matrix4 LocalMatrix(m_Columns[0], m_Columns[1], m_Columns[2], m_Columns[3]);

			// Normalize the matrix.
			if (IsZero(LocalMatrix[3][3]))
				return false;

			for (onyxU8 i = 0; i < 4; ++i)
			{
				for (onyxU8 j = 0; j < 4; ++j)
				{
					LocalMatrix[i][j] /= LocalMatrix[3][3];
				}
			}

			outTranslation[0] = LocalMatrix[3][0];
			outTranslation[1] = LocalMatrix[3][1];
			outTranslation[2] = LocalMatrix[3][2];

			LocalMatrix[3] = Vector4<Scalar>(0.0f, 0.0f, 0.0f, LocalMatrix[3][3]);

			//Vector3<Scalar> Skew;
			Vector3<Scalar> Row[3];

			for (onyxU8 i = 0; i < 3; ++i)
			{
				for (onyxU8 j = 0; j < 3; ++j)
				{
					Row[i][j] = LocalMatrix[i][j];
				}
			}

			// Compute X scale factor and normalize first row.
			outScale[0] = Row[0].Length();// v3Length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<Scalar>(1));

			// Now, compute Y scale and normalize 2nd row.
			outScale[1] = static_cast<Scalar>(Row[1].Length());
			Row[1] = detail::scale(Row[1], static_cast<Scalar>(1));

			// Next, get Z scale and normalize 3rd row.
			outScale[2] = static_cast<Scalar>(Row[2].Length());
			Row[2] = detail::scale(Row[2], static_cast<Scalar>(1));
			
			// rotation as Rotor3
			
			Scalar root, trace = Row[0][0] + Row[1][1] + Row[2][2];

			Bivector3<Scalar> bivector;
			if (trace > static_cast<Scalar>(0))
			{
				root = sqrt(trace + static_cast<Scalar>(1));
				outRotation.m_Scalar = static_cast<Scalar>(0.5) * root;

				root = static_cast<Scalar>(0.5) / root;
				bivector[0] = root * (Row[1][2] - Row[2][1]);
				bivector[1] = root * (Row[2][0] - Row[0][2]);
				bivector[2] = root * (Row[0][1] - Row[1][0]);
				outRotation.m_Bivector = bivector;
			}
			else
			{
				int i, j, k = 0;
				static const onyxU8 Next[3] = { 1, 2, 0 };
				i = 0;
				if (Row[1][1] > Row[0][0]) i = 1;
				if (Row[2][2] > Row[i][i]) i = 2;
				j = Next[i];
				k = Next[j];

				root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<Scalar>(1.0));

				bivector[i] = static_cast<Scalar>(0.5) * root;
				root = static_cast<Scalar>(0.5) / root;
				bivector[j] = root * (Row[i][j] + Row[j][i]);
				bivector[k] = root * (Row[i][k] + Row[k][i]);
				outRotation.m_Scalar = root * (Row[j][k] - Row[k][j]);
				outRotation.m_Bivector = bivector;
			}

			 return true;
        }

    private:
		Vector4<Scalar> m_Columns[4];
    };

	template <typename ScalarT>
	Matrix4<ScalarT> Rotor3<ScalarT>::ToMatrix4() const
	{
		return Matrix4{ ToMatrix3() };
	}

	template<typename Scalar>
	Vector4<Scalar> operator*(const Matrix4<Scalar>& m, const Vector4<Scalar>& v)
	{
		const Vector4<Scalar> Mov0(v[0]);
		const Vector4<Scalar> Mov1(v[1]);
		const Vector4<Scalar> Mul0 = m[0] * Mov0;
		const Vector4<Scalar> Mul1 = m[1] * Mov1;
		const Vector4<Scalar> Add0 = Mul0 + Mul1;
		const Vector4<Scalar> Mov2(v[2]);
		const Vector4<Scalar> Mov3(v[3]);
		const Vector4<Scalar> Mul2 = m[2] * Mov2;
		const Vector4<Scalar> Mul3 = m[3] * Mov3;
		const Vector4<Scalar> Add1 = Mul2 + Mul3;
		const Vector4<Scalar> Add2 = Add0 + Add1;
		return Add2;
	}
}
