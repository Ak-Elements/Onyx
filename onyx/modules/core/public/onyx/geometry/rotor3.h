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
    constexpr Rotor3() = default;

    /*
     * Constructs a rotor3 from 3 euler angles
     * Rotation order is YXZ
     * x - Pitch angle to rotate around X axis
     * y - Yaw angle to rotate around Y axis
     * z - Roll angle to rotate around Z axis
     */
    constexpr Rotor3( Vector3f32 pitchYawRoll ) {
        const ScalarT cp = std::cos( pitchYawRoll.X * ScalarT( 0.5 ) );
        const ScalarT cy = std::cos( pitchYawRoll.Y * ScalarT( 0.5 ) );
        const ScalarT cr = std::cos( pitchYawRoll.Z * ScalarT( 0.5 ) );
        const ScalarT sp = std::sin( pitchYawRoll.X * ScalarT( 0.5 ) );
        const ScalarT sy = std::sin( pitchYawRoll.Y * ScalarT( 0.5 ) );
        const ScalarT sr = std::sin( pitchYawRoll.Z * ScalarT( 0.5 ) );

        Rotor3 rotor;
        m_scalar = cp * cy * cr + sp * sy * sr;
        m_bivector[ 0 ] = sy * sp * cr - cy * cp * sr;      // XY - Roll
        m_bivector[ 1 ] = sy * cp * cr - cy * sp * sr;      // XZ - Yaw
        m_bivector[ 2 ] = -( cy * sp * cr + sy * cp * sr ); // YZ - Pitch
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
    constexpr Rotor3( units::Radians< ScalarT > pitch,
                                      units::Radians< ScalarT > yaw,
                                      units::Radians< ScalarT > roll ) {
        Rotor3 rotationX( pitch.count(), Bivector3< ScalarT >::yzUnit() );
        Rotor3 rotationY( yaw.count(), Bivector3< ScalarT >::zxUnit() );
        Rotor3 rotationZ( roll.count(), Bivector3< ScalarT >::xyUnit() );

        auto rotor = ( rotationY * rotationX * rotationZ ).normalized();
        m_scalar = rotor.m_scalar;
        m_bivector = rotor.m_bivector;
    }

    static constexpr Rotor3 angleAxis( ScalarT angle, const Vector3< ScalarT >& vec ) {
        const ScalarT s = static_cast< ScalarT >( std::sin( angle * static_cast< ScalarT >( 0.5f ) ) );
        return Rotor3( std::cos( angle * static_cast< ScalarT >( 0.5f ) ), vec * s );
    }

    constexpr Rotor3( ScalarT angle, const Bivector3< ScalarT > bivector )
        : m_scalar( std::cos( angle / 2.0f ) ) {
        ScalarT sina = std::sin( angle / 2.0f );

        m_bivector[ 0 ] = -sina * bivector[ 0 ];
        m_bivector[ 1 ] = -sina * bivector[ 1 ];
        m_bivector[ 2 ] = -sina * bivector[ 2 ];
    }

    constexpr Rotor3 operator*( const Rotor3& rhs ) const {
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

    constexpr Vector3< ScalarT > rotate( const Vector3< ScalarT >& vec ) const {
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

    constexpr Matrix3< ScalarT > toMatrix3() const {
        return toMatrix3x3();
    }

    constexpr Matrix3< ScalarT > toMatrix3x3() const {
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

    constexpr Matrix4< ScalarT > toMatrix4() const;

    constexpr Vector3< ScalarT > toEulerAngles() const {
        // Rotor -> standard quaternion mapping
        const ScalarT w = m_scalar;

        const ScalarT x = -m_bivector[ 2 ]; // pitch (X)
        const ScalarT y = m_bivector[ 1 ];  // yaw   (Y)
        const ScalarT z = -m_bivector[ 0 ]; // roll  (Z)

        // Matches:
        // R = Ry(yaw) * Rx(pitch) * Rz(roll)
        const ScalarT sinPitch = ScalarT( 2 ) * ( w * x - y * z );
        const ScalarT pitch = std::asin( std::clamp( sinPitch, ScalarT( -1 ), ScalarT( 1 ) ) );

        const ScalarT sinYaw = ScalarT( 2 ) * ( w * y + x * z );
        const ScalarT cosYaw = ScalarT( 1 ) - ScalarT( 2 ) * ( x * x + y * y );

        const ScalarT yaw = std::atan2( sinYaw, cosYaw );

        const ScalarT sinRoll = ScalarT( 2 ) * ( w * z + x * y );
        const ScalarT cosRoll = ScalarT( 1 ) - ScalarT( 2 ) * ( x * x + z * z );

        const ScalarT roll = std::atan2( sinRoll, cosRoll );

        return { pitch, yaw, roll };
    }

    constexpr ScalarT lengthSquared() const {
        return m_scalar * m_scalar + m_bivector[ 0 ] * m_bivector[ 0 ] + m_bivector[ 1 ] * m_bivector[ 1 ] +
               m_bivector[ 2 ] * m_bivector[ 2 ];
    }
    // Length
    constexpr ScalarT length() const { return std::sqrt( lengthSquared() ); }

    constexpr void normalize() {
        ScalarT l = length();
        m_scalar /= l;
        m_bivector[ 0 ] /= l;
        m_bivector[ 1 ] /= l;
        m_bivector[ 2 ] /= l;
    }

    constexpr Rotor3 normalized() const {
        Rotor3 r = *this;
        r.normalize();
        return r;
    }

    constexpr bool operator==( const Rotor3& rhs ) const {
        return isEqual( m_scalar, rhs.m_scalar ) && ( m_bivector == rhs.m_bivector );
    }

  private:
    ScalarT m_scalar = 1;
    Bivector3< ScalarT > m_bivector;
};
} // namespace onyx
