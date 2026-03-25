#pragma once

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
     * z - Roll angle to rotate around Z axis
     */
    static Rotor3 fromEulerAngles( const Vector3< ScalarT >& eulerAngles ) {
        Vector3< ScalarT > cos{ std::cos( eulerAngles[ 0 ] * ScalarT( 0.5 ) ),
                                std::cos( eulerAngles[ 1 ] * ScalarT( 0.5 ) ),
                                std::cos( eulerAngles[ 2 ] * ScalarT( 0.5 ) ) };
        Vector3< ScalarT > sin = { std::sin( eulerAngles[ 0 ] * ScalarT( 0.5 ) ),
                                   std::sin( eulerAngles[ 1 ] * ScalarT( 0.5 ) ),
                                   std::sin( eulerAngles[ 2 ] * ScalarT( 0.5 ) ) };

        Rotor3 rotor;
        rotor.m_scalar = cos[ 0 ] * cos[ 1 ] * cos[ 2 ] + sin[ 0 ] * sin[ 1 ] * sin[ 2 ];
        rotor.m_bivector[ 0 ] = sin[ 0 ] * cos[ 1 ] * cos[ 2 ] - cos[ 0 ] * sin[ 1 ] * sin[ 2 ];
        rotor.m_bivector[ 1 ] = cos[ 0 ] * sin[ 1 ] * cos[ 2 ] + sin[ 0 ] * cos[ 1 ] * sin[ 2 ];
        rotor.m_bivector[ 2 ] = cos[ 0 ] * cos[ 1 ] * sin[ 2 ] - sin[ 0 ] * sin[ 1 ] * cos[ 2 ];

        return rotor;
    }

    /*
     * Construct a rotor3 from 3 euler angles in degrees
     * Pitch - Angle to rotate around X axis
     * Yaw - Angle to rotate around Y axis
     * Roll - Angle to rotate around Z axis
     */
    static Rotor3 fromEulerAnglesDegrees( ScalarT pitch, ScalarT yaw, ScalarT roll ) {
        Rotor3 rotationX( numericCast< ScalarT >( ToRadians( pitch ) ), Bivector3< ScalarT >::yzUnit() );
        Rotor3 rotationY( numericCast< ScalarT >( ToRadians( yaw ) ), Bivector3< ScalarT >::zxUnit() );
        Rotor3 rotationZ( numericCast< ScalarT >( ToRadians( roll ) ), Bivector3< ScalarT >::xyUnit() );

        return rotationX * rotationY * rotationZ;
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

        return ( rotationX * rotationY * rotationZ ).Normalized();
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
        Matrix3< ScalarT > result;
        ScalarT qxx( m_bivector[ 0 ] * m_bivector[ 0 ] );
        ScalarT qyy( m_bivector[ 1 ] * m_bivector[ 1 ] );
        ScalarT qzz( m_bivector[ 2 ] * m_bivector[ 2 ] );
        ScalarT qxz( m_bivector[ 0 ] * m_bivector[ 2 ] );
        ScalarT qxy( m_bivector[ 0 ] * m_bivector[ 1 ] );
        ScalarT qyz( m_bivector[ 1 ] * m_bivector[ 2 ] );
        ScalarT qwx( m_scalar * m_bivector[ 0 ] );
        ScalarT qwy( m_scalar * m_bivector[ 1 ] );
        ScalarT qwz( m_scalar * m_bivector[ 2 ] );

        result[ 0 ][ 0 ] = ScalarT( 1 ) - ScalarT( 2 ) * ( qyy + qzz );
        result[ 0 ][ 1 ] = ScalarT( 2 ) * ( qxy + qwz );
        result[ 0 ][ 2 ] = ScalarT( 2 ) * ( qxz - qwy );

        result[ 1 ][ 0 ] = ScalarT( 2 ) * ( qxy - qwz );
        result[ 1 ][ 1 ] = ScalarT( 1 ) - ScalarT( 2 ) * ( qxx + qzz );
        result[ 1 ][ 2 ] = ScalarT( 2 ) * ( qyz + qwx );

        result[ 2 ][ 0 ] = ScalarT( 2 ) * ( qxz + qwy );
        result[ 2 ][ 1 ] = ScalarT( 2 ) * ( qyz - qwx );
        result[ 2 ][ 2 ] = ScalarT( 1 ) - ScalarT( 2 ) * ( qxx + qyy );

        return result;
    }

    Matrix4< ScalarT > toMatrix4() const;

    Vector3< ScalarT > toEulerAngles() const {
        ScalarT const y = static_cast< ScalarT >( 2 ) *
                          ( m_bivector[ 1 ] * m_bivector[ 2 ] + m_scalar * m_bivector[ 0 ] );
        ScalarT const x = m_scalar * m_scalar - m_bivector[ 0 ] * m_bivector[ 0 ] - m_bivector[ 1 ] * m_bivector[ 1 ] +
                          m_bivector[ 2 ] * m_bivector[ 2 ];

        ScalarT pitch = isZero( x ) && isZero( y ) ? static_cast< ScalarT >( static_cast< ScalarT >( 2 ) *
                                                                             std::atan2( m_bivector[ 0 ], m_scalar ) )
                                                   : static_cast< ScalarT >( std::atan2( y, x ) );
        ScalarT yaw = std::asin( std::clamp( static_cast< ScalarT >( -2 ) *
                                                 ( m_bivector[ 0 ] * m_bivector[ 2 ] - m_scalar * m_bivector[ 1 ] ),
                                             static_cast< ScalarT >( -1 ),
                                             static_cast< ScalarT >( 1 ) ) );
        ScalarT roll = static_cast< ScalarT >( std::atan2(
            static_cast< ScalarT >( 2 ) * ( m_bivector[ 0 ] * m_bivector[ 1 ] + m_scalar * m_bivector[ 2 ] ),
            m_scalar * m_scalar + m_bivector[ 0 ] * m_bivector[ 0 ] - m_bivector[ 1 ] * m_bivector[ 1 ] -
                m_bivector[ 2 ] * m_bivector[ 2 ] ) );

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

  private:
    ScalarT m_scalar = 1;
    Bivector3< ScalarT > m_bivector;
};
} // namespace onyx