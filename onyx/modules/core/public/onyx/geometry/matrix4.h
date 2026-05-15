#pragma once

#include <onyx/geometry/rotor3.h>

namespace onyx {
namespace detail {
/// Make a linear combination of two vectors and return the result.
// result = (a * ascl) + (b * bscl)
template < typename Scalar >
Vector3< Scalar > combine( const Vector3< Scalar >& a, const Vector3< Scalar >& b, Scalar ascl, Scalar bscl ) {
    return ( a * ascl ) + ( b * bscl );
}

template < typename Scalar >
Vector3< Scalar > scale( const Vector3< Scalar >& v, Scalar desiredLength ) {
    return v * desiredLength / static_cast< Scalar >( v.length() );
}
} // namespace detail

template < typename Scalar >
struct ONYX_ALIGN( 16 ) Matrix4 {
  public:
    Matrix4()
        : m_columns{
              Vector4< Scalar >( 1, 0, 0, 0 ),
              Vector4< Scalar >( 0, 1, 0, 0 ),
              Vector4< Scalar >( 0, 0, 1, 0 ),
              Vector4< Scalar >( 0, 0, 0, 1 ),
          } {}

    explicit constexpr Matrix4( Scalar identityScalar )
        : m_columns{
              Vector4< Scalar >( identityScalar, 0, 0, 0 ),
              Vector4< Scalar >( 0, identityScalar, 0, 0 ),
              Vector4< Scalar >( 0, 0, identityScalar, 0 ),
              Vector4< Scalar >( 0, 0, 0, identityScalar ),
          } {}

    constexpr Matrix4( Scalar m00,
                       Scalar m01,
                       Scalar m02,
                       Scalar m03,
                       Scalar m10,
                       Scalar m11,
                       Scalar m12,
                       Scalar m13,
                       Scalar m20,
                       Scalar m21,
                       Scalar m22,
                       Scalar m23,
                       Scalar m30,
                       Scalar m31,
                       Scalar m32,
                       Scalar m33 )
        : m_columns{ { m00, m01, m02, m03 }, { m10, m11, m12, m13 }, { m20, m21, m22, m23 }, { m30, m31, m32, m33 } } {}

    constexpr Matrix4( const Vector4< Scalar >& column0,
                       const Vector4< Scalar >& column1,
                       const Vector4< Scalar >& column2,
                       const Vector4< Scalar >& column3 )
        : m_columns{ column0, column1, column2, column3 } {}

    explicit constexpr Matrix4( Matrix3< Scalar > mat3 )
        : m_columns{
              Vector4< Scalar >( mat3[ 0 ], 0 ),
              Vector4< Scalar >( mat3[ 1 ], 0 ),
              Vector4< Scalar >( mat3[ 2 ], 0 ),
              Vector4< Scalar >( 0, 0, 0, 1 ),
          } {}

    Vector4< Scalar >& operator[]( uint8_t columnIndex ) { return m_columns[ columnIndex ]; }
    const Vector4< Scalar >& operator[]( uint8_t columnIndex ) const { return m_columns[ columnIndex ]; }

    // Scalar& operator[](uint8_t columnIndex, uint8_t rowIndex) { return m_Columns[columnIndex][rowIndex]; }
    // Scalar operator[](uint8_t columnIndex, uint8_t rowIndex) const { return m_Columns[columnIndex][rowIndex]; }

    Matrix4< Scalar > operator*( const Matrix4& rhs ) const {
        const Vector4< Scalar >& srcA0 = m_columns[ 0 ];
        const Vector4< Scalar >& srcA1 = m_columns[ 1 ];
        const Vector4< Scalar >& srcA2 = m_columns[ 2 ];
        const Vector4< Scalar >& srcA3 = m_columns[ 3 ];

        const Vector4< Scalar >& srcB0 = rhs[ 0 ];
        const Vector4< Scalar >& srcB1 = rhs[ 1 ];
        const Vector4< Scalar >& srcB2 = rhs[ 2 ];
        const Vector4< Scalar >& srcB3 = rhs[ 3 ];

        Matrix4< Scalar > result;

        Vector4< Scalar > tmp;
        tmp = srcA0 * srcB0[ 0 ];
        tmp += srcA1 * srcB0[ 1 ];
        tmp += srcA2 * srcB0[ 2 ];
        tmp += srcA3 * srcB0[ 3 ];
        result[ 0 ] = tmp;
        tmp = srcA0 * srcB1[ 0 ];
        tmp += srcA1 * srcB1[ 1 ];
        tmp += srcA2 * srcB1[ 2 ];
        tmp += srcA3 * srcB1[ 3 ];
        result[ 1 ] = tmp;
        tmp = srcA0 * srcB2[ 0 ];
        tmp += srcA1 * srcB2[ 1 ];
        tmp += srcA2 * srcB2[ 2 ];
        tmp += srcA3 * srcB2[ 3 ];
        result[ 2 ] = tmp;
        tmp = srcA0 * srcB3[ 0 ];
        tmp += srcA1 * srcB3[ 1 ];
        tmp += srcA2 * srcB3[ 2 ];
        tmp += srcA3 * srcB3[ 3 ];
        result[ 3 ] = tmp;

        return result;
    }

    Matrix4< Scalar > operator*( Scalar scalar ) const {
        return Matrix4< Scalar >( m_columns[ 0 ] * scalar,
                                  m_columns[ 1 ] * scalar,
                                  m_columns[ 2 ] * scalar,
                                  m_columns[ 3 ] * scalar );
    }

    Matrix4< Scalar > inverse() const {
        Scalar coef00 = m_columns[ 2 ][ 2 ] * m_columns[ 3 ][ 3 ] - m_columns[ 3 ][ 2 ] * m_columns[ 2 ][ 3 ];
        Scalar coef02 = m_columns[ 1 ][ 2 ] * m_columns[ 3 ][ 3 ] - m_columns[ 3 ][ 2 ] * m_columns[ 1 ][ 3 ];
        Scalar coef03 = m_columns[ 1 ][ 2 ] * m_columns[ 2 ][ 3 ] - m_columns[ 2 ][ 2 ] * m_columns[ 1 ][ 3 ];

        Scalar coef04 = m_columns[ 2 ][ 1 ] * m_columns[ 3 ][ 3 ] - m_columns[ 3 ][ 1 ] * m_columns[ 2 ][ 3 ];
        Scalar coef06 = m_columns[ 1 ][ 1 ] * m_columns[ 3 ][ 3 ] - m_columns[ 3 ][ 1 ] * m_columns[ 1 ][ 3 ];
        Scalar coef07 = m_columns[ 1 ][ 1 ] * m_columns[ 2 ][ 3 ] - m_columns[ 2 ][ 1 ] * m_columns[ 1 ][ 3 ];

        Scalar coef08 = m_columns[ 2 ][ 1 ] * m_columns[ 3 ][ 2 ] - m_columns[ 3 ][ 1 ] * m_columns[ 2 ][ 2 ];
        Scalar coef10 = m_columns[ 1 ][ 1 ] * m_columns[ 3 ][ 2 ] - m_columns[ 3 ][ 1 ] * m_columns[ 1 ][ 2 ];
        Scalar coef11 = m_columns[ 1 ][ 1 ] * m_columns[ 2 ][ 2 ] - m_columns[ 2 ][ 1 ] * m_columns[ 1 ][ 2 ];

        Scalar coef12 = m_columns[ 2 ][ 0 ] * m_columns[ 3 ][ 3 ] - m_columns[ 3 ][ 0 ] * m_columns[ 2 ][ 3 ];
        Scalar coef14 = m_columns[ 1 ][ 0 ] * m_columns[ 3 ][ 3 ] - m_columns[ 3 ][ 0 ] * m_columns[ 1 ][ 3 ];
        Scalar coef15 = m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 3 ] - m_columns[ 2 ][ 0 ] * m_columns[ 1 ][ 3 ];

        Scalar coef16 = m_columns[ 2 ][ 0 ] * m_columns[ 3 ][ 2 ] - m_columns[ 3 ][ 0 ] * m_columns[ 2 ][ 2 ];
        Scalar coef18 = m_columns[ 1 ][ 0 ] * m_columns[ 3 ][ 2 ] - m_columns[ 3 ][ 0 ] * m_columns[ 1 ][ 2 ];
        Scalar coef19 = m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 2 ] - m_columns[ 2 ][ 0 ] * m_columns[ 1 ][ 2 ];

        Scalar coef20 = m_columns[ 2 ][ 0 ] * m_columns[ 3 ][ 1 ] - m_columns[ 3 ][ 0 ] * m_columns[ 2 ][ 1 ];
        Scalar coef22 = m_columns[ 1 ][ 0 ] * m_columns[ 3 ][ 1 ] - m_columns[ 3 ][ 0 ] * m_columns[ 1 ][ 1 ];
        Scalar coef23 = m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 1 ] - m_columns[ 2 ][ 0 ] * m_columns[ 1 ][ 1 ];

        Vector4< Scalar > fac0( coef00, coef00, coef02, coef03 );
        Vector4< Scalar > fac1( coef04, coef04, coef06, coef07 );
        Vector4< Scalar > fac2( coef08, coef08, coef10, coef11 );
        Vector4< Scalar > fac3( coef12, coef12, coef14, coef15 );
        Vector4< Scalar > fac4( coef16, coef16, coef18, coef19 );
        Vector4< Scalar > fac5( coef20, coef20, coef22, coef23 );

        Vector4< Scalar > vec0( m_columns[ 1 ][ 0 ], m_columns[ 0 ][ 0 ], m_columns[ 0 ][ 0 ], m_columns[ 0 ][ 0 ] );
        Vector4< Scalar > vec1( m_columns[ 1 ][ 1 ], m_columns[ 0 ][ 1 ], m_columns[ 0 ][ 1 ], m_columns[ 0 ][ 1 ] );
        Vector4< Scalar > vec2( m_columns[ 1 ][ 2 ], m_columns[ 0 ][ 2 ], m_columns[ 0 ][ 2 ], m_columns[ 0 ][ 2 ] );
        Vector4< Scalar > vec3( m_columns[ 1 ][ 3 ], m_columns[ 0 ][ 3 ], m_columns[ 0 ][ 3 ], m_columns[ 0 ][ 3 ] );

        Vector4< Scalar > inv0( vec1 * fac0 - vec2 * fac1 + vec3 * fac2 );
        Vector4< Scalar > inv1( vec0 * fac0 - vec2 * fac3 + vec3 * fac4 );
        Vector4< Scalar > inv2( vec0 * fac1 - vec1 * fac3 + vec3 * fac5 );
        Vector4< Scalar > inv3( vec0 * fac2 - vec1 * fac4 + vec2 * fac5 );

        Vector4< Scalar > signA( +1, -1, +1, -1 );
        Vector4< Scalar > signB( -1, +1, -1, +1 );
        Matrix4 inverse( inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB );

        Vector4< Scalar > row0( inverse[ 0 ][ 0 ], inverse[ 1 ][ 0 ], inverse[ 2 ][ 0 ], inverse[ 3 ][ 0 ] );

        Vector4< Scalar > dot0( m_columns[ 0 ] * row0 );
        Scalar dot1 = ( dot0[ 0 ] + dot0[ 1 ] ) + ( dot0[ 2 ] + dot0[ 3 ] );

        Scalar oneOverDeterminant = static_cast< Scalar >( 1 ) / dot1;

        return inverse * oneOverDeterminant;
    }

    bool decompose( Vector3< Scalar >& outTranslation, Rotor3< Scalar >& outRotation, Vector3< Scalar >& outScale ) {
        Matrix4 localMatrix( m_columns[ 0 ], m_columns[ 1 ], m_columns[ 2 ], m_columns[ 3 ] );

        // Normalize the matrix.
        if( isZero( localMatrix[ 3 ][ 3 ] ) )
            return false;

        for( uint8_t i = 0; i < 4; ++i ) {
            for( uint8_t j = 0; j < 4; ++j ) {
                localMatrix[ i ][ j ] /= localMatrix[ 3 ][ 3 ];
            }
        }

        outTranslation[ 0 ] = localMatrix[ 3 ][ 0 ];
        outTranslation[ 1 ] = localMatrix[ 3 ][ 1 ];
        outTranslation[ 2 ] = localMatrix[ 3 ][ 2 ];

        localMatrix[ 3 ] = Vector4< Scalar >( 0.0f, 0.0f, 0.0f, localMatrix[ 3 ][ 3 ] );

        // Vector3<Scalar> Skew;
        Vector3< Scalar > row[ 3 ];

        for( uint8_t i = 0; i < 3; ++i ) {
            for( uint8_t j = 0; j < 3; ++j ) {
                row[ i ][ j ] = localMatrix[ i ][ j ];
            }
        }

        // Compute X scale factor and normalize first row.
        outScale[ 0 ] = row[ 0 ].length(); // v3Length(Row[0]);
        row[ 0 ] = detail::scale( row[ 0 ], static_cast< Scalar >( 1 ) );

        // Now, compute Y scale and normalize 2nd row.
        outScale[ 1 ] = static_cast< Scalar >( row[ 1 ].length() );
        row[ 1 ] = detail::scale( row[ 1 ], static_cast< Scalar >( 1 ) );

        // Next, get Z scale and normalize 3rd row.
        outScale[ 2 ] = static_cast< Scalar >( row[ 2 ].length() );
        row[ 2 ] = detail::scale( row[ 2 ], static_cast< Scalar >( 1 ) );

        // rotation as Rotor3

        Scalar root, trace = row[ 0 ][ 0 ] + row[ 1 ][ 1 ] + row[ 2 ][ 2 ];

        Bivector3< Scalar > bivector;
        if( trace > static_cast< Scalar >( 0 ) ) {
            root = sqrt( trace + static_cast< Scalar >( 1 ) );
            outRotation.m_scalar = static_cast< Scalar >( 0.5 ) * root;
            root = static_cast< Scalar >( 0.5 ) / root; // root = 1/(4*scalar)

            // antisymmetric differences with correct index order
            bivector[ 0 ] = root * ( row[ 1 ][ 0 ] - row[ 0 ][ 1 ] );
            bivector[ 1 ] = root * ( row[ 2 ][ 0 ] - row[ 0 ][ 2 ] );
            bivector[ 2 ] = root * ( row[ 2 ][ 1 ] - row[ 1 ][ 2 ] );
            outRotation.m_bivector = bivector;
        } else {
            static const uint8_t Next[ 3 ] = { 1, 2, 0 };
            // Maps standard cyclic slot (e23=0, e31=1, e12=2) to bivector index
            constexpr uint8_t Slot[ 3 ] = { 2, 1, 0 };
            // Sign correction for bivector layout vs standard
            constexpr Scalar Sign[ 3 ] = { Scalar( 1 ), Scalar( -1 ), Scalar( 1 ) };

            int i = 0;
            if( row[ 1 ][ 1 ] > row[ 0 ][ 0 ] )
                i = 1;
            if( row[ 2 ][ 2 ] > row[ i ][ i ] )
                i = 2;
            int j = Next[ i ];
            int k = Next[ j ];

            root = sqrt( row[ i ][ i ] - row[ j ][ j ] - row[ k ][ k ] + static_cast< Scalar >( 1 ) );
            bivector[ Slot[ i ] ] = Sign[ i ] * static_cast< Scalar >( 0.5 ) * root;
            root = static_cast< Scalar >( 0.5 ) / root;

            bivector[ Slot[ j ] ] = Sign[ j ] * root * ( row[ i ][ j ] + row[ j ][ i ] );
            bivector[ Slot[ k ] ] = Sign[ k ] * root * ( row[ i ][ k ] + row[ k ][ i ] );

            outRotation.m_scalar = ( i == 1 ) ? root * ( row[ j ][ k ] - row[ k ][ j ] )
                                              : root * ( row[ k ][ j ] - row[ j ][ k ] );

            outRotation.m_bivector = bivector;
        }

        // R and -R represent the same rotation; keep scalar non-negative for canonical form
        if( outRotation.m_scalar < static_cast< Scalar >( 0 ) ) {
            outRotation.m_scalar = -outRotation.m_scalar;
            outRotation.m_bivector[ 0 ] = -outRotation.m_bivector[ 0 ];
            outRotation.m_bivector[ 1 ] = -outRotation.m_bivector[ 1 ];
            outRotation.m_bivector[ 2 ] = -outRotation.m_bivector[ 2 ];
        }

        return true;
    }

  private:
    Vector4< Scalar > m_columns[ 4 ];
};

template < typename ScalarT >
Matrix4< ScalarT > Rotor3< ScalarT >::toMatrix4() const {
    return Matrix4{ toMatrix3() };
}

template < typename Scalar >
Vector4< Scalar > operator*( const Matrix4< Scalar >& m, const Vector4< Scalar >& v ) {
    const Vector4< Scalar > mov0( v[ 0 ] );
    const Vector4< Scalar > mov1( v[ 1 ] );
    const Vector4< Scalar > mul0 = m[ 0 ] * mov0;
    const Vector4< Scalar > mul1 = m[ 1 ] * mov1;
    const Vector4< Scalar > add0 = mul0 + mul1;
    const Vector4< Scalar > mov2( v[ 2 ] );
    const Vector4< Scalar > mov3( v[ 3 ] );
    const Vector4< Scalar > mul2 = m[ 2 ] * mov2;
    const Vector4< Scalar > mul3 = m[ 3 ] * mov3;
    const Vector4< Scalar > add1 = mul2 + mul3;
    const Vector4< Scalar > add2 = add0 + add1;
    return add2;
}
} // namespace onyx
