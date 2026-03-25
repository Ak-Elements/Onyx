#pragma once

namespace onyx {
template < typename T >
concept IsRatio = requires {
    // Must have the static members Numerator and Denominator
    { T::Numerator } -> std::convertible_to< int64_t >;
    { T::Denominator } -> std::convertible_to< int64_t >;
};

template < int64_t TNum, int64_t TDemon = 1 >
struct Ratio {
    using Type = Ratio< TNum, TDemon >;
    constexpr static inline int64_t Numerator = TNum;
    constexpr static inline int64_t Denominator = TDemon;
};

namespace Detail {
template < typename T >
constexpr T abs( const T& value ) {
    return value < 0 ? -value : value;
}

constexpr auto gcd( int64_t first, int64_t second ) -> int64_t {
    if ( first == 0 and second == 0 ) {
        return 1; // avoids division by 0 in ratio_less
    }

    first = Detail::abs( first );
    second = Detail::abs( second );

    while ( second != 0 ) {
        const int64_t temp = first;
        first = second;
        second = temp % second;
    }

    return first;
}

template < int64_t TMultiplier, int64_t TMultiplicand >
constexpr inline bool IsMultiplyOverflow = []() {
    return Detail::abs( TMultiplier ) <=
           ::std::numeric_limits< int64_t >::max() / ( TMultiplicand == 0 ? 1 : Detail::abs( TMultiplicand ) );
}();

template < int64_t TMultiplier, int64_t TMultiplicand >
struct Multiple : std::integral_constant< int64_t, TMultiplier * TMultiplicand > {
    static_assert( IsMultiplyOverflow< TMultiplier, TMultiplicand >, "integer arithmetic overflow" );
};

template < typename TMultiplier, typename TMultiplicand >
struct RatioMultiply {
    constexpr static int64_t GreatestCommonDivisor1 = gcd( TMultiplier::Numerator, TMultiplicand::Denominator );
    constexpr static int64_t GreatestCommonDivisor2 = gcd( TMultiplicand::Numerator, TMultiplier::Denominator );

    using Numerator = Detail::Multiple< TMultiplier::Numerator / GreatestCommonDivisor1,
                                        TMultiplicand::Numerator / GreatestCommonDivisor2 >;
    using Denominator = Detail::Multiple< TMultiplier::Denominator / GreatestCommonDivisor2,
                                          TMultiplicand::Denominator / GreatestCommonDivisor1 >;
    using Type = Ratio< Numerator::value, Denominator::value >;
};
} // namespace Detail

template < typename TRatio >
using RatioInvert = Ratio< TRatio::Denominator, TRatio::Numerator >;

template < typename TMultiplier, typename TMultiplicand >
using RatioMultiply = Detail::RatioMultiply< TMultiplier, TMultiplicand >::Type;

template < typename TDivisor, typename TDividend >
using RatioDivide = RatioMultiply< TDivisor, RatioInvert< TDividend > >;
} // namespace onyx
