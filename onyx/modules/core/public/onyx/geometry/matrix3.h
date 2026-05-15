#pragma once

namespace onyx {
template < typename Scalar >
struct Matrix3 {
  public:
    // constexpr Matrix3<Scalar> ZERO = Matrix3<Scalar>(0, 0, 0, 0, 0, 0, 0, 0, 0);
    // static constexpr Matrix3 Identity;
    // constexpr Matrix3<Scalar> INVERSE = Matrix3<Scalar>(-1, -1, -1, -1, -1, -1, -1, -1, -1);

    constexpr Matrix3()
        : m_columns{ Vector3< Scalar >( 1, 0, 0 ), Vector3< Scalar >( 0, 1, 0 ), Vector3< Scalar >( 0, 0, 1 ) } {}

    constexpr Matrix3( Scalar m00,
                       Scalar m01,
                       Scalar m02,
                       Scalar m10,
                       Scalar m11,
                       Scalar m12,
                       Scalar m20,
                       Scalar m21,
                       Scalar m22 )
        : m_columns{ { m00, m01, m02 }, { m10, m11, m12 }, { m20, m21, m22 } } {}

    constexpr Matrix3( Vector3< Scalar > column0, Vector3< Scalar > column1, Vector3< Scalar > column2 )
        : m_columns{ column0, column1, column2 } {}

    Matrix3< Scalar > operator*( const Matrix3& rhs ) const {
        const Vector3< Scalar >& srcA0 = m_columns[ 0 ];
        const Vector3< Scalar >& srcA1 = m_columns[ 1 ];
        const Vector3< Scalar >& srcA2 = m_columns[ 2 ];

        const Vector3< Scalar >& srcB0 = rhs[ 0 ];
        const Vector3< Scalar >& srcB1 = rhs[ 1 ];
        const Vector3< Scalar >& srcB2 = rhs[ 2 ];

        Matrix3< Scalar > result;

        Vector3< Scalar > tmp;
        tmp = srcA0 * srcB0[ 0 ];
        tmp += srcA1 * srcB0[ 1 ];
        tmp += srcA2 * srcB0[ 2 ];
        result[ 0 ] = tmp;
        tmp = srcA0 * srcB1[ 0 ];
        tmp += srcA1 * srcB1[ 1 ];
        tmp += srcA2 * srcB1[ 2 ];
        result[ 1 ] = tmp;
        tmp = srcA0 * srcB2[ 0 ];
        tmp += srcA1 * srcB2[ 1 ];
        tmp += srcA2 * srcB2[ 2 ];
        result[ 2 ] = tmp;

        return result;
    }

    Vector3< Scalar >& operator[]( uint8_t columnIndex ) { return m_columns[ columnIndex ]; }
    const Vector3< Scalar >& operator[]( uint8_t columnIndex ) const { return m_columns[ columnIndex ]; }

    void inverse() {
        // Calculate the determinant
        Scalar det = m_columns[ 0 ][ 0 ] *
                         ( m_columns[ 1 ][ 1 ] * m_columns[ 2 ][ 2 ] - m_columns[ 1 ][ 2 ] * m_columns[ 2 ][ 1 ] ) -
                     m_columns[ 0 ][ 1 ] *
                         ( m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 2 ] - m_columns[ 1 ][ 2 ] * m_columns[ 2 ][ 0 ] ) +
                     m_columns[ 0 ][ 2 ] *
                         ( m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 1 ] - m_columns[ 1 ][ 1 ] * m_columns[ 2 ][ 0 ] );

        if( isZero( det ) ) // Check for singular matrix
        {
            throw std::runtime_error( "Matrix is not invertible (determinant is zero)" );
        }

        Scalar invDet = static_cast< Scalar >( 1 ) / det;

        // Calculate the cofactors and the adjugate matrix
        Matrix3< Scalar > adj;
        adj[ 0 ][ 0 ] = ( m_columns[ 1 ][ 1 ] * m_columns[ 2 ][ 2 ] - m_columns[ 1 ][ 2 ] * m_columns[ 2 ][ 1 ] ) *
                        invDet;
        adj[ 0 ][ 1 ] = -( m_columns[ 0 ][ 1 ] * m_columns[ 2 ][ 2 ] - m_columns[ 0 ][ 2 ] * m_columns[ 2 ][ 1 ] ) *
                        invDet;
        adj[ 0 ][ 2 ] = ( m_columns[ 0 ][ 1 ] * m_columns[ 1 ][ 2 ] - m_columns[ 0 ][ 2 ] * m_columns[ 1 ][ 1 ] ) *
                        invDet;

        adj[ 1 ][ 0 ] = -( m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 2 ] - m_columns[ 1 ][ 2 ] * m_columns[ 2 ][ 0 ] ) *
                        invDet;
        adj[ 1 ][ 1 ] = ( m_columns[ 0 ][ 0 ] * m_columns[ 2 ][ 2 ] - m_columns[ 0 ][ 2 ] * m_columns[ 2 ][ 0 ] ) *
                        invDet;
        adj[ 1 ][ 2 ] = -( m_columns[ 0 ][ 0 ] * m_columns[ 1 ][ 2 ] - m_columns[ 0 ][ 2 ] * m_columns[ 1 ][ 0 ] ) *
                        invDet;

        adj[ 2 ][ 0 ] = ( m_columns[ 1 ][ 0 ] * m_columns[ 2 ][ 1 ] - m_columns[ 1 ][ 1 ] * m_columns[ 2 ][ 0 ] ) *
                        invDet;
        adj[ 2 ][ 1 ] = -( m_columns[ 0 ][ 0 ] * m_columns[ 2 ][ 1 ] - m_columns[ 0 ][ 1 ] * m_columns[ 2 ][ 0 ] ) *
                        invDet;
        adj[ 2 ][ 2 ] = ( m_columns[ 0 ][ 0 ] * m_columns[ 1 ][ 1 ] - m_columns[ 0 ][ 1 ] * m_columns[ 1 ][ 0 ] ) *
                        invDet;

        // Update the matrix with the inverse
        *this = adj;
    }

  private:
    Vector3< Scalar > m_columns[ 3 ];
};

template < typename Scalar >
Vector3< Scalar > operator*( const Matrix3< Scalar >& m, const Vector3< Scalar >& v ) {
    return Vector3< Scalar >( v[ 0 ] * m[ 0 ][ 0 ] + v[ 1 ] * m[ 1 ][ 0 ] + v[ 2 ] * m[ 2 ][ 0 ],
                              v[ 0 ] * m[ 0 ][ 1 ] + v[ 1 ] * m[ 1 ][ 1 ] + v[ 2 ] * m[ 2 ][ 1 ],
                              v[ 0 ] * m[ 0 ][ 2 ] + v[ 1 ] * m[ 1 ][ 2 ] + v[ 2 ] * m[ 2 ][ 2 ] );
}

template < typename Scalar >
Vector3< Scalar > operator*( const Vector3< Scalar >& v, const Matrix3< Scalar >& m ) {
    return Vector3< Scalar >( v[ 0 ] * m[ 0 ][ 0 ] + v[ 1 ] * m[ 0 ][ 1 ] + v[ 2 ] * m[ 0 ][ 2 ],
                              v[ 0 ] * m[ 1 ][ 0 ] + v[ 1 ] * m[ 1 ][ 1 ] + v[ 2 ] * m[ 1 ][ 2 ],
                              v[ 0 ] * m[ 2 ][ 0 ] + v[ 1 ] * m[ 2 ][ 1 ] + v[ 2 ] * m[ 2 ][ 2 ] );
}
} // namespace onyx
