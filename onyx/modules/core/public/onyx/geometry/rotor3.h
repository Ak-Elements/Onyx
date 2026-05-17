#pragma once

#include <onyx/units/units.h>

namespace onyx {
template < typename ScalarT >
struct Matrix3;

template < typename ScalarT >
struct Matrix4;
/*
 * Rotor is the mathematical equivalent to Quaternions.
 * Rotors are the correct mathematical representation of Quaternions, they can be used as 1 to 1 replacements.
 */
template < typename ScalarT >
class Rotor3 {
    friend struct Matrix4< ScalarT >;

  public:
    constexpr Rotor3() = default;

    /*
     * Constructs a rotor3 from 3 euler angles
     * Rotation order is YXZ
     * x - Pitch angle to rotate around X axis
     * y - Yaw angle to rotate around Y axis
     * z - Roll angle to rotate around Z axis
     */
    constexpr Rotor3( Vector3< ScalarT > pitchYawRoll ) {
        const ScalarT cp = std::cos( pitchYawRoll.X * ScalarT( 0.5 ) );
        const ScalarT cy = std::cos( pitchYawRoll.Y * ScalarT( 0.5 ) );
        const ScalarT cr = std::cos( pitchYawRoll.Z * ScalarT( 0.5 ) );
        const ScalarT sp = std::sin( pitchYawRoll.X * ScalarT( 0.5 ) );
        const ScalarT sy = std::sin( pitchYawRoll.Y * ScalarT( 0.5 ) );
        const ScalarT sr = std::sin( pitchYawRoll.Z * ScalarT( 0.5 ) );

        Rotor3 rotor;
        m_scalar = cp * cy * cr + sp * sy * sr;
        m_bivector.XY = sy * sp * cr - cy * cp * sr;      // XY - Roll
        m_bivector.XZ = sy * cp * cr - cy * sp * sr;      // XZ - Yaw
        m_bivector.YZ = -( cy * sp * cr + sy * cp * sr ); // YZ - Pitch
    }

    /*
     * Constructs a rotor3 from 3 euler angles
     * Rotation order is YXZ
     */
    constexpr Rotor3( units::Degrees< ScalarT > pitch, units::Degrees< ScalarT > yaw, units::Degrees< ScalarT > roll )
        : Rotor3( quantityCast< units::Radians< ScalarT > >( pitch ),
                  quantityCast< units::Radians< ScalarT > >( yaw ),
                  quantityCast< units::Radians< ScalarT > >( roll ) ) {}

    /*
     * Constructs a rotor3 from 3 euler angles
     * Rotation order is YXZ
     */
    constexpr Rotor3( units::Radians< ScalarT > pitch, units::Radians< ScalarT > yaw, units::Radians< ScalarT > roll ) {
        Rotor3 rotationX( pitch.count(), Bivector3< ScalarT >::yzUnit() );
        Rotor3 rotationY( yaw.count(), Bivector3< ScalarT >::zxUnit() );
        Rotor3 rotationZ( roll.count(), Bivector3< ScalarT >::xyUnit() );

        auto rotor = ( rotationY * rotationX * rotationZ ).normalized();
        m_scalar = rotor.m_scalar;
        m_bivector = rotor.m_bivector;
    }

    constexpr Rotor3( ScalarT angle, const Bivector3< ScalarT > bivector )
        : m_scalar( std::cos( angle * 0.5f ) ) {
        ScalarT sina = std::sin( angle * 0.5f );

        m_bivector.XY = -sina * bivector.XY;
        m_bivector.XZ = -sina * bivector.XZ;
        m_bivector.YZ = -sina * bivector.YZ;
    }

    constexpr Rotor3( Vector3< ScalarT > from, const Vector3< ScalarT > to ) {
        m_scalar = ScalarT( 1 ) + to.dot( from );
        m_bivector = to.wedge( from );
        normalize();
    }

    constexpr Rotor3 operator*( const Rotor3& rhs ) const {
        Rotor3 result;

        result.m_scalar = ( m_scalar * rhs.m_scalar ) - ( m_bivector.XY * rhs.m_bivector.XY ) -
                          ( m_bivector.XZ * rhs.m_bivector.XZ ) - ( m_bivector.YZ * rhs.m_bivector.YZ );
        result.m_bivector.XY = ( m_bivector.XY * rhs.m_scalar ) + ( m_scalar * rhs.m_bivector.XY ) +
                               ( m_bivector.YZ * rhs.m_bivector.XZ ) - ( m_bivector.XZ * rhs.m_bivector.YZ );
        result.m_bivector.XZ = ( m_bivector.XZ * rhs.m_scalar ) + ( m_scalar * rhs.m_bivector.XZ ) -
                               ( m_bivector.YZ * rhs.m_bivector.XY ) + ( m_bivector.XY * rhs.m_bivector.YZ );
        result.m_bivector.YZ = ( m_bivector.YZ * rhs.m_scalar ) + ( m_scalar * rhs.m_bivector.YZ ) +
                               ( m_bivector.XZ * rhs.m_bivector.XY ) - ( m_bivector.XY * rhs.m_bivector.XZ );

        return result;
    }

    constexpr Vector3< ScalarT > rotate( const Vector3< ScalarT >& vec ) const {
        // q = P x
        Vector3< ScalarT > q;
        q.X = m_scalar * vec.X + vec.Y * m_bivector.XY + vec.Z * m_bivector.XZ;
        q.Y = m_scalar * vec.Y - vec.X * m_bivector.XY + vec.Z * m_bivector.YZ;
        q.Z = m_scalar * vec.Z - vec.X * m_bivector.XZ - vec.Y * m_bivector.YZ;

        ScalarT q012 = vec.X * m_bivector.YZ - vec.Y * m_bivector.XZ + vec.Z * m_bivector.XY; // trivector

        // r = q P*
        return { m_scalar * q.X + q.Y * m_bivector.XY + q.Z * m_bivector.XZ + q012 * m_bivector.YZ,
                 m_scalar * q.Y - q.X * m_bivector.XY - q012 * m_bivector.XZ + q.Z * m_bivector.YZ,
                 m_scalar * q.Z + q012 * m_bivector.XY - q.X * m_bivector.XZ - q.Y * m_bivector.YZ };
    }

    constexpr Matrix3< ScalarT > toMatrix3() const {
        const ScalarT scalar = m_scalar;

        const ScalarT yx = m_bivector.YZ; // YZ
        const ScalarT xz = m_bivector.XZ; // XZ
        const ScalarT xy = m_bivector.XY; // XY

        const ScalarT scalarSquared = scalar * scalar;

        const ScalarT yxSquared = yx * yx;
        const ScalarT xzSquared = xz * xz;
        const ScalarT xySquared = xy * xy;

        const ScalarT lengthSquared = yxSquared + xzSquared + xySquared;
        const ScalarT s = scalarSquared - lengthSquared;
        const ScalarT k = ScalarT( 2 );

        Matrix3< ScalarT > result;

        result[ 0 ][ 0 ] = s + k * yxSquared;
        result[ 1 ][ 0 ] = k * ( -yx * xz + scalar * xy );
        result[ 2 ][ 0 ] = k * ( yx * xy + scalar * xz );

        result[ 0 ][ 1 ] = k * ( -yx * xz - scalar * xy );
        result[ 1 ][ 1 ] = s + k * xzSquared;
        result[ 2 ][ 1 ] = k * ( -xz * xy + scalar * yx );

        result[ 0 ][ 2 ] = k * ( yx * xy - scalar * xz );
        result[ 1 ][ 2 ] = k * ( -xz * xy - scalar * yx );
        result[ 2 ][ 2 ] = s + k * xySquared;

        return result;
    }

    constexpr Matrix4< ScalarT > toMatrix4() const;

    constexpr Vector3< ScalarT > toEulerAngles() const {
        const ScalarT w = m_scalar;

        const ScalarT x = m_bivector.YZ; // pitch (X)
        const ScalarT y = m_bivector.XZ; // yaw   (Y)
        const ScalarT z = m_bivector.XY; // roll  (Z)

        // R = Ry(yaw) * Rx(pitch) * Rz(roll)
        const ScalarT sinPitch = ScalarT( -2 ) * ( w * x - y * z );
        const ScalarT pitch = std::asin( std::clamp( sinPitch, ScalarT( -1 ), ScalarT( 1 ) ) );

        const ScalarT sinYaw = ScalarT( 2 ) * ( w * y + x * z );
        const ScalarT cosYaw = ScalarT( 1 ) - ScalarT( 2 ) * ( x * x + y * y );

        const ScalarT yaw = std::atan2( sinYaw, cosYaw );

        const ScalarT sinRoll = ScalarT( -2 ) * ( w * z + x * y );
        const ScalarT cosRoll = ScalarT( 1 ) - ScalarT( 2 ) * ( x * x + z * z );

        const ScalarT roll = std::atan2( sinRoll, cosRoll );

        return { pitch, yaw, roll };
    }

    constexpr ScalarT lengthSquared() const {
        return m_scalar * m_scalar + m_bivector.XY * m_bivector.XY + m_bivector.XZ * m_bivector.XZ +
               m_bivector.YZ * m_bivector.YZ;
    }
    // Length
    constexpr ScalarT length() const { return std::sqrt( lengthSquared() ); }

    constexpr void normalize() {
        ScalarT l = length();
        m_scalar /= l;
        m_bivector.XY /= l;
        m_bivector.XZ /= l;
        m_bivector.YZ /= l;
    }

    constexpr Rotor3 normalized() const {
        Rotor3 r = *this;
        r.normalize();
        return r;
    }

    constexpr bool operator==( const Rotor3& rhs ) const {
        return isEqual( m_scalar, rhs.m_scalar ) && ( m_bivector == rhs.m_bivector );
    }

    constexpr bool operator!=( const Rotor3& rhs ) const { return ( *this == rhs ) == false; }

  private:
    ScalarT m_scalar = 1;
    Bivector3< ScalarT > m_bivector;
};
} // namespace onyx
