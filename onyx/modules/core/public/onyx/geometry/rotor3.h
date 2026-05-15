#pragma once

#include <onyx/units/units.h>

namespace onyx {
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
    Rotor3() = default;

    /*
     * Construct a rotor3 from 3 euler angles in radians
     * x - Pitch angle to rotate around X axis
     * y - Yaw angle to rotate around Y axis
     */
    static Rotor3 fromEulerAngles( Vector3f32 pitchYawRoll ) {
        const ScalarT cp = std::cos( pitchYawRoll.X * ScalarT( 0.5 ) );
        const ScalarT cy = std::cos( pitchYawRoll.Y * ScalarT( 0.5 ) );
        const ScalarT cr = std::cos( pitchYawRoll.Z * ScalarT( 0.5 ) );
        const ScalarT sp = std::sin( pitchYawRoll.X * ScalarT( 0.5 ) );
        const ScalarT sy = std::sin( pitchYawRoll.Y * ScalarT( 0.5 ) );
        const ScalarT sr = std::sin( pitchYawRoll.Z * ScalarT( 0.5 ) );

        Rotor3 rotor;
        rotor.m_scalar = cp * cy * cr - sp * sy * sr;
        rotor.m_bivector[ 0 ] = -( cp * cy * sr + sp * sy * cr ); // XY - Roll
        rotor.m_bivector[ 1 ] = ( cp * sy * cr - sp * cy * sr );  // XZ - Yaw
        rotor.m_bivector[ 2 ] = -( sp * cy * cr + cp * sy * sr ); // YZ - Pitch
        return rotor.normalized();
    }

    /*
     * Construct a rotor3 from 3 euler angles in degrees
     * Pitch - Angle to rotate around X axis
     * Yaw - Angle to rotate around Y axis
     * Roll - Angle to rotate around Z axis
     */
    static Rotor3 fromEulerAnglesDegrees( units::DegreesF32 pitch, units::DegreesF32 yaw, units::DegreesF32 roll ) {
        Rotor3 rotationX( quantityCast< units::RadiansF32 >( pitch ).count(), Bivector3< ScalarT >::yzUnit() );
        Rotor3 rotationY( quantityCast< units::RadiansF32 >( yaw ).count(), Bivector3< ScalarT >::zxUnit() );
        Rotor3 rotationZ( quantityCast< units::RadiansF32 >( roll ).count(), Bivector3< ScalarT >::xyUnit() );

        return ( rotationX * rotationY * rotationZ ).normalized();
    }

    /*
     * Construct a rotor3 from 3 euler angles in radians
     * Pitch - Angle to rotate around X axis
     * Yaw - Angle to rotate around Y axis
     * Roll - Angle to rotate around Z axis
     */
    static Rotor3 fromEulerAngles( ScalarT pitch, ScalarT yaw, ScalarT roll ) {
        Rotor3 rotationX( pitch, Bivector3< ScalarT >::yzUnit() );
        Rotor3 rotationY( yaw, Bivector3< ScalarT >::zxUnit() );
        Rotor3 rotationZ( roll, Bivector3< ScalarT >::xyUnit() );

        return ( rotationX * rotationY * rotationZ ).normalized();
    }

    static Rotor3 angleAxis( ScalarT angle, const Vector3< ScalarT >& vec ) {
        const ScalarT s = static_cast< ScalarT >( std::sin( angle * static_cast< ScalarT >( 0.5f ) ) );
        return Rotor3( std::cos( angle * static_cast< ScalarT >( 0.5f ) ), vec * s );
    }

    Rotor3( ScalarT angle, const Bivector3< ScalarT > bivector )
        : m_scalar( std::cos( angle / 2.0f ) ) {
        ScalarT sina = std::sin( angle / 2.0f );

        m_bivector[ 0 ] = -sina * bivector[ 0 ];
        m_bivector[ 1 ] = -sina * bivector[ 1 ];
        m_bivector[ 2 ] = -sina * bivector[ 2 ];
    }

    Rotor3 operator*( const Rotor3& rhs ) const {
        Rotor3 result;

        result.m_scalar = ( m_scalar * rhs.m_scalar ) - ( m_bivector[ 0 ] * rhs.m_bivector[ 0 ] ) -
                          ( m_bivector[ 1 ] * rhs.m_bivector[ 1 ] ) - ( m_bivector[ 2 ] * rhs.m_bivector[ 2 ] );
        result.m_bivector[ 0 ] = ( m_bivector[ 0 ] * rhs.m_scalar ) + ( m_scalar * rhs.m_bivector[ 0 ] ) +
                                 ( m_bivector[ 2 ] * rhs.m_bivector[ 1 ] ) - ( m_bivector[ 1 ] * rhs.m_bivector[ 2 ] );
        result.m_bivector[ 1 ] = ( m_bivector[ 1 ] * rhs.m_scalar ) + ( m_scalar * rhs.m_bivector[ 1 ] ) -
                                 ( m_bivector[ 2 ] * rhs.m_bivector[ 0 ] ) + ( m_bivector[ 0 ] * rhs.m_bivector[ 2 ] );
        result.m_bivector[ 2 ] = ( m_bivector[ 2 ] * rhs.m_scalar ) + ( m_scalar * rhs.m_bivector[ 2 ] ) +
                                 ( m_bivector[ 1 ] * rhs.m_bivector[ 0 ] ) - ( m_bivector[ 0 ] * rhs.m_bivector[ 1 ] );

        return result;
    }

    Vector3< ScalarT > rotate( const Vector3< ScalarT >& vec ) const {
        // q = P x
        Vector3< ScalarT > q;
        q[ 0 ] = m_scalar * vec[ 0 ] + vec[ 1 ] * m_bivector[ 0 ] + vec[ 2 ] * m_bivector[ 1 ];
        q[ 1 ] = m_scalar * vec[ 1 ] - vec[ 0 ] * m_bivector[ 0 ] + vec[ 2 ] * m_bivector[ 2 ];
        q[ 2 ] = m_scalar * vec[ 2 ] - vec[ 0 ] * m_bivector[ 1 ] - vec[ 1 ] * m_bivector[ 2 ];

        ScalarT q012 = vec[ 0 ] * m_bivector[ 2 ] - vec[ 1 ] * m_bivector[ 1 ] +
                       vec[ 2 ] * m_bivector[ 0 ]; // trivector

        // r = q P*
        Vector3< ScalarT > result;
        result[ 0 ] = m_scalar * q[ 0 ] + q[ 1 ] * m_bivector[ 0 ] + q[ 2 ] * m_bivector[ 1 ] + q012 * m_bivector[ 2 ];
        result[ 1 ] = m_scalar * q[ 1 ] - q[ 0 ] * m_bivector[ 0 ] - q012 * m_bivector[ 1 ] + q[ 2 ] * m_bivector[ 2 ];
        result[ 2 ] = m_scalar * q[ 2 ] + q012 * m_bivector[ 0 ] - q[ 0 ] * m_bivector[ 1 ] - q[ 1 ] * m_bivector[ 2 ];

        return result;
    }

    Matrix3< ScalarT > toMatrix3() const {
        // Vector3< ScalarT > v0 = rotate( Vector3< ScalarT >( 1, 0, 0 ) );
        // Vector3< ScalarT > v1 = rotate( Vector3< ScalarT >( 0, 1, 0 ) );
        // Vector3< ScalarT > v2 = rotate( Vector3< ScalarT >( 0, 0, 1 ) );
        // return Matrix3< ScalarT >( v0, v1, v2 );
        return toMatrix3x3();
    }

    Matrix3< ScalarT > toMatrix3x3() const {
        const ScalarT a = m_scalar;

        const ScalarT bx = -m_bivector[ 2 ]; // YZ
        const ScalarT by = m_bivector[ 1 ];  // XZ
        const ScalarT bz = -m_bivector[ 0 ]; // XY

        const ScalarT a2 = a * a;

        const ScalarT bx2 = bx * bx;
        const ScalarT by2 = by * by;
        const ScalarT bz2 = bz * bz;

        const ScalarT b2 = bx2 + by2 + bz2;
        const ScalarT s = a2 - b2;
        const ScalarT k = ScalarT( 2 );

        Matrix3< ScalarT > result;

        result[ 0 ][ 0 ] = s + k * bx2;
        result[ 1 ][ 0 ] = k * ( bx * by - a * bz );
        result[ 2 ][ 0 ] = k * ( bx * bz + a * by );

        result[ 0 ][ 1 ] = k * ( bx * by + a * bz );
        result[ 1 ][ 1 ] = s + k * by2;
        result[ 2 ][ 1 ] = k * ( by * bz - a * bx );

        result[ 0 ][ 2 ] = k * ( bx * bz - a * by );
        result[ 1 ][ 2 ] = k * ( by * bz + a * bx );
        result[ 2 ][ 2 ] = s + k * bz2;

        return result;
    }

    Matrix4< ScalarT > toMatrix4() const;

    Vector3< ScalarT > toEulerAngles() const {
        // Rotor -> standard quaternion mapping
        const ScalarT w = m_scalar;

        const ScalarT x = -m_bivector[ 2 ]; // pitch (X)
        const ScalarT y = -m_bivector[ 1 ]; // yaw   (Y)
        const ScalarT z = -m_bivector[ 0 ]; // roll  (Z)

        //
        // Matches:
        // R = Rx(pitch) * Ry(yaw) * Rz(roll)
        //

        // =========================
        // Pitch (X)
        // =========================
        const ScalarT sinPitch = ScalarT( 2 ) * ( w * x + y * z );

        const ScalarT cosPitch = ScalarT( 1 ) - ScalarT( 2 ) * ( x * x + y * y );

        const ScalarT pitch = std::atan2( sinPitch, cosPitch );

        // =========================
        // Yaw (Y)
        // =========================
        const ScalarT sinYaw = ScalarT( -2 ) * ( w * y - z * x );

        const ScalarT yaw = std::asin( std::clamp( sinYaw, ScalarT( -1 ), ScalarT( 1 ) ) );

        // =========================
        // Roll (Z)
        // =========================
        const ScalarT sinRoll = ScalarT( 2 ) * ( w * z + x * y );

        const ScalarT cosRoll = ScalarT( 1 ) - ScalarT( 2 ) * ( y * y + z * z );

        const ScalarT roll = std::atan2( sinRoll, cosRoll );

        return { pitch, yaw, roll };
    }
    ScalarT lengthSquared() const {
        return m_scalar * m_scalar + m_bivector[ 0 ] * m_bivector[ 0 ] + m_bivector[ 1 ] * m_bivector[ 1 ] +
               m_bivector[ 2 ] * m_bivector[ 2 ];
    }
    // Length
    ScalarT length() const { return std::sqrt( lengthSquared() ); }

    void normalize() {
        ScalarT l = length();
        m_scalar /= l;
        m_bivector[ 0 ] /= l;
        m_bivector[ 1 ] /= l;
        m_bivector[ 2 ] /= l;
    }

    Rotor3 normalized() const {
        Rotor3 r = *this;
        r.normalize();
        return r;
    }

    bool operator==( const Rotor3& rhs ) const {
        return isEqual( m_scalar, rhs.m_scalar ) && ( m_bivector == rhs.m_bivector );
    }

  public:
    ScalarT m_scalar = 1;
    Bivector3< ScalarT > m_bivector;
};
} // namespace onyx
