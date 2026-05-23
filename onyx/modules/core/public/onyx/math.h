#pragma once

namespace onyx {
template < typename Scalar >
constexpr bool isEqual( Scalar lhs, Scalar rhs ) {
    if constexpr( std::is_floating_point_v< Scalar > ) {
        constexpr Scalar Epsilon = std::numeric_limits< Scalar >::epsilon();
        const Scalar scale = std::max( std::abs( lhs ), std::abs( rhs ) );
        return std::abs( lhs - rhs ) <= std::max( Epsilon, Epsilon * scale );
    } else
        return lhs == rhs;
}

template < typename Scalar, typename = std::enable_if_t< std::is_floating_point_v< Scalar > > >
constexpr bool isEqual( Scalar lhs, Scalar rhs, Scalar epsilon ) {
    return std::abs( lhs - rhs ) <= epsilon;
}

template < typename T >
constexpr bool isZero( T value ) {
    if constexpr( std::is_floating_point_v< T > ) {
        constexpr T Epsilon = std::numeric_limits< T >::epsilon();
        return ( value >= -Epsilon ) && ( value <= Epsilon );
    } else
        return value == T( 0 );
}

template < typename Scalar, typename = std::enable_if_t< std::is_floating_point< Scalar >::value > >
constexpr bool isZero( Scalar value, Scalar epsilon ) {
    return ( value >= -epsilon ) && ( value <= epsilon );
}

template < typename ToT, typename FromT >
constexpr ToT numericCast( FromT value ) {
    ONYX_ASSERT( static_cast< ToT >( value ) >= std::numeric_limits< ToT >::lowest() &&
                 static_cast< ToT >( value ) <= std::numeric_limits< ToT >::max() );
    return static_cast< ToT >( value );
}

#if !ONYX_IS_FINAL
template < typename Scalar >
constexpr bool isAdditionSafe( Scalar lhs, Scalar rhs ) {
    const bool isOverflow = ( rhs > 0 ) && ( lhs > ( std::numeric_limits< Scalar >::max() - rhs ) );

    if constexpr( std::is_signed_v< Scalar > ) {
        const bool isUnderflow = ( rhs < 0 ) && ( lhs < ( std::numeric_limits< Scalar >::lowest() - rhs ) );
        return ( isOverflow == false ) && ( isUnderflow == false );
    } else {
        return ( isOverflow == false );
    }
}

template < typename Scalar >
constexpr bool isSubtractionSafe( Scalar lhs, Scalar rhs ) {
    const Scalar min = std::numeric_limits< Scalar >::lowest();

    const bool isUnderflow = ( rhs > 0 ) && ( lhs < ( min + rhs ) );

    if constexpr( std::is_signed_v< Scalar > ) {
        const Scalar max = std::numeric_limits< Scalar >::max();
        const bool isOverflow = ( rhs < 0 ) && ( lhs > ( max + rhs ) );
        return ( isOverflow == false ) && ( isUnderflow == false );
    } else {
        return ( isUnderflow == false );
    }
}

template < typename Scalar >
constexpr bool isMultiplicationSafe( Scalar lhs, Scalar rhs ) {
    constexpr Scalar min = std::numeric_limits< Scalar >::lowest();

    if constexpr( std::is_signed_v< Scalar > ) {
        if( isEqual( lhs, Scalar( -1 ) ) && ( rhs == min ) )
            return false;

        if( isEqual( rhs, Scalar( -1 ) ) && ( lhs == min ) )
            return false;
    }

    const bool isOverflow = ( rhs != 0 ) && ( lhs > ( std::numeric_limits< Scalar >::max() / rhs ) );
    const bool isUnderflow = ( rhs != 0 ) && ( lhs < ( std::numeric_limits< Scalar >::lowest() / rhs ) );

    return ( isOverflow == false ) && ( isUnderflow == false );
}

template < typename Scalar >
constexpr bool isDivisionSafe( Scalar lhs, Scalar rhs ) {
    if constexpr( std::is_signed_v< Scalar > == false ) {
        if( isEqual( rhs, Scalar( -1 ) ) && ( lhs == std::numeric_limits< Scalar >::lowest() ) )
            return false;
    }

    return true;
}
#endif
} // namespace onyx
