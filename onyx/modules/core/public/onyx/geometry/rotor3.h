#pragma once

namespace Onyx
{
    template <typename ScalarT>
    struct Matrix4;
    /*
     * Rotor is the mathematical equivalent to Quaternions.
     * Rotors are the correct mathematical representation of Quaternions, they can be used as 1 to 1 replacements.
     */
    template <typename ScalarT>
    class Rotor3
    {
        friend struct Matrix4<ScalarT>;
    public:
        Rotor3() = default;

        /*
         * Construct a rotor3 from 3 euler angles in radians
         * x - Pitch angle to rotate around X axis
         * y - Yaw angle to rotate around Y axis
         * z - Roll angle to rotate around Z axis
         */
        static Rotor3 FromEulerAngles(const Vector3<ScalarT>& eulerAngles)
        {
            Vector3<ScalarT> cos{ std::cos(eulerAngles[0] * ScalarT(0.5)), std::cos(eulerAngles[1] * ScalarT(0.5)), std::cos(eulerAngles[2] * ScalarT(0.5)) };
            Vector3<ScalarT> sin = { std::sin(eulerAngles[0] * ScalarT(0.5)), std::sin(eulerAngles[1] * ScalarT(0.5)), std::sin(eulerAngles[2] * ScalarT(0.5)) };

            Rotor3 rotor;
            rotor.m_Scalar = cos[0] * cos[1] * cos[2] + sin[0] * sin[1] * sin[2];
            rotor.m_Bivector[0] = sin[0] * cos[1] * cos[2] - cos[0] * sin[1] * sin[2];
            rotor.m_Bivector[1] = cos[0] * sin[1] * cos[2] + sin[0] * cos[1] * sin[2];
            rotor.m_Bivector[2] = cos[0] * cos[1] * sin[2] - sin[0] * sin[1] * cos[2];

            return rotor;
        }

        /*
         * Construct a rotor3 from 3 euler angles in degrees
         * Pitch - Angle to rotate around X axis
         * Yaw - Angle to rotate around Y axis
         * Roll - Angle to rotate around Z axis
         */
        static Rotor3 FromEulerAnglesDegrees(ScalarT pitch, ScalarT yaw, ScalarT roll)
        {
            Rotor3 rotationX(numeric_cast<ScalarT>(ToRadians(pitch)), Bivector3<ScalarT>::YZ_Unit());
            Rotor3 rotationY(numeric_cast<ScalarT>(ToRadians(yaw)), Bivector3<ScalarT>::ZX_Unit());
            Rotor3 rotationZ(numeric_cast<ScalarT>(ToRadians(roll)), Bivector3<ScalarT>::XY_Unit());

            return rotationX * rotationY * rotationZ;
        }

        /*
         * Construct a rotor3 from 3 euler angles in radians
         * Pitch - Angle to rotate around X axis
         * Yaw - Angle to rotate around Y axis
         * Roll - Angle to rotate around Z axis
         */
        static Rotor3 FromEulerAngles(ScalarT pitch, ScalarT yaw, ScalarT roll)
        {
            Rotor3 rotationX(pitch, Bivector3<ScalarT>::YZ_Unit());
            Rotor3 rotationY(yaw, Bivector3<ScalarT>::ZX_Unit());
            Rotor3 rotationZ(roll, Bivector3<ScalarT>::XY_Unit());

            return (rotationX * rotationY * rotationZ).Normalized();
        }

        static Rotor3 AngleAxis(ScalarT angle, const Vector3<ScalarT>& vec)
        {
            const ScalarT s = static_cast<ScalarT>(std::sin(angle * static_cast<ScalarT>(0.5f)));
            return Rotor3(std::cos(angle * static_cast<ScalarT>(0.5f)), vec * s);
        }

        Rotor3(ScalarT angle, const Bivector3<ScalarT> bivector)
            : m_Scalar(std::cos(angle / 2.0f))
        {
            ScalarT sina = std::sin(angle / 2.0f);

            m_Bivector[0] = -sina * bivector[0];
            m_Bivector[1] = -sina * bivector[1];
            m_Bivector[2] = -sina * bivector[2];
        }

        Rotor3 operator*(const Rotor3& rhs) const
        {
            Rotor3 result;
            
            result.m_Scalar = (m_Scalar * rhs.m_Scalar) - (m_Bivector[0] * rhs.m_Bivector[0]) - (m_Bivector[1] * rhs.m_Bivector[1]) - (m_Bivector[2] * rhs.m_Bivector[2]);
            result.m_Bivector[0] = (m_Bivector[0] * rhs.m_Scalar) + (m_Scalar * rhs.m_Bivector[0]) + (m_Bivector[2] * rhs.m_Bivector[1]) - (m_Bivector[1] * rhs.m_Bivector[2]);
            result.m_Bivector[1] = (m_Bivector[1] * rhs.m_Scalar) + (m_Scalar * rhs.m_Bivector[1]) - (m_Bivector[2] * rhs.m_Bivector[0]) + (m_Bivector[0] * rhs.m_Bivector[2]);
            result.m_Bivector[2] = (m_Bivector[2] * rhs.m_Scalar) + (m_Scalar * rhs.m_Bivector[2]) + (m_Bivector[1] * rhs.m_Bivector[0]) - (m_Bivector[0] * rhs.m_Bivector[1]);

            return result;
        }

        Vector3<ScalarT> rotate(const Vector3<ScalarT>& vec) const
        {
            // q = P x
            Vector3<ScalarT> q;
            q[0] = m_Scalar * vec[0] + vec[1] * m_Bivector[0] + vec[2] * m_Bivector[1];
            q[1] = m_Scalar * vec[1] - vec[0] * m_Bivector[0] + vec[2] * m_Bivector[2];
            q[2] = m_Scalar * vec[2] - vec[0] * m_Bivector[1] - vec[1] * m_Bivector[2];

            ScalarT q012 = vec[0] * m_Bivector[2] - vec[1] * m_Bivector[1] + vec[2] * m_Bivector[0]; // trivector

            // r = q P*
            Vector3<ScalarT> result;
            result[0] = m_Scalar * q[0] + q[1] * m_Bivector[0] + q[2] * m_Bivector[1] + q012 * m_Bivector[2];
            result[1] = m_Scalar * q[1] - q[0] * m_Bivector[0] - q012 * m_Bivector[1] + q[2] * m_Bivector[2];
            result[2] = m_Scalar * q[2] + q012 * m_Bivector[0] - q[0] * m_Bivector[1] - q[1] * m_Bivector[2];

            return result;
        }

        Matrix3<ScalarT> ToMatrix3() const
        {
            Matrix3<ScalarT> Result;
            ScalarT qxx(m_Bivector[0] * m_Bivector[0]);
            ScalarT qyy(m_Bivector[1] * m_Bivector[1]);
            ScalarT qzz(m_Bivector[2] * m_Bivector[2]);
            ScalarT qxz(m_Bivector[0] * m_Bivector[2]);
            ScalarT qxy(m_Bivector[0] * m_Bivector[1]);
            ScalarT qyz(m_Bivector[1] * m_Bivector[2]);
            ScalarT qwx(m_Scalar * m_Bivector[0]);
            ScalarT qwy(m_Scalar * m_Bivector[1]);
            ScalarT qwz(m_Scalar * m_Bivector[2]);

            Result[0][0] = ScalarT(1) - ScalarT(2) * (qyy + qzz);
            Result[0][1] = ScalarT(2) * (qxy + qwz);
            Result[0][2] = ScalarT(2) * (qxz - qwy);

            Result[1][0] = ScalarT(2) * (qxy - qwz);
            Result[1][1] = ScalarT(1) - ScalarT(2) * (qxx + qzz);
            Result[1][2] = ScalarT(2) * (qyz + qwx);

            Result[2][0] = ScalarT(2) * (qxz + qwy);
            Result[2][1] = ScalarT(2) * (qyz - qwx);
            Result[2][2] = ScalarT(1) - ScalarT(2) * (qxx + qyy);

            return Result;
        }

        Matrix4<ScalarT> ToMatrix4() const;

        Vector3<ScalarT> ToEulerAngles() const
        {
            ScalarT const y = static_cast<ScalarT>(2) * (m_Bivector[1] * m_Bivector[2] + m_Scalar * m_Bivector[0]);
            ScalarT const x = m_Scalar * m_Scalar - m_Bivector[0] * m_Bivector[0] - m_Bivector[1] * m_Bivector[1] + m_Bivector[2] * m_Bivector[2];

            ScalarT pitch = IsZero(x) && IsZero(y) ? static_cast<ScalarT>(static_cast<ScalarT>(2) * std::atan2(m_Bivector[0], m_Scalar)) : static_cast<ScalarT>(std::atan2(y, x));
            ScalarT yaw = std::asin(std::clamp(static_cast<ScalarT>(-2) * (m_Bivector[0] * m_Bivector[2] - m_Scalar * m_Bivector[1]), static_cast<ScalarT>(-1), static_cast<ScalarT>(1)));
            ScalarT roll = static_cast<ScalarT>(std::atan2(static_cast<ScalarT>(2) * (m_Bivector[0] * m_Bivector[1] + m_Scalar * m_Bivector[2]), m_Scalar * m_Scalar + m_Bivector[0] * m_Bivector[0] - m_Bivector[1] * m_Bivector[1] - m_Bivector[2] * m_Bivector[2]));

            return { pitch, yaw, roll };
        }

        ScalarT LengthSquared() const
        {
            return m_Scalar * m_Scalar + m_Bivector[0] * m_Bivector[0] + m_Bivector[1] * m_Bivector[1] + m_Bivector[2] * m_Bivector[2];
        }
        // Length
        ScalarT Length() const
        {
            return std::sqrt(LengthSquared());
        }

        void Normalize()
        {
            ScalarT l = Length();
            m_Scalar /= l;
            m_Bivector[0] /= l;
            m_Bivector[1] /= l;
            m_Bivector[2] /= l;
        }
        
        Rotor3 Normalized() const
        {
            Rotor3 r = *this;
            r.Normalize();
            return r;
        }

        bool operator==(const Rotor3& rhs) const
        {
            return IsEqual(m_Scalar, rhs.m_Scalar) && (m_Bivector == rhs.m_Bivector);
        }

    private:
        ScalarT m_Scalar = 1;
        Bivector3<ScalarT> m_Bivector;
    };
}