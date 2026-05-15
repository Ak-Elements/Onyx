#pragma once

#include <onyx/geometry/vector.h>
#include <onyx/units/ratio.h>

namespace onyx {

template < typename T >
concept StorageQuantity = requires( const T& obj ) {
    typename T::RepresentType;
    typename T::PeriodType;
    requires( Numeric< typename T::RepresentType > );
    { obj.count() } -> std::same_as< typename T::RepresentType >;
};

template < Numeric Rep, typename Period >
struct Quantity;

template < StorageQuantity TTo, StorageQuantity TFrom >
ONYX_NO_DISCARD constexpr auto quantityCast( const TFrom& quantity ) noexcept -> TTo;

template < Numeric Rep, typename Period >
struct Quantity {
  public:
    using RepresentType = Rep;
    using PeriodType = typename Period::Type;

    constexpr Quantity() = default;

    template < typename Rep2 >
    requires( std::convertible_to< const Rep2&, RepresentType > and
              ( std::is_floating_point_v< RepresentType > or not std::is_floating_point_v< Rep2 > ) )
    constexpr explicit Quantity( const Rep2& value ) noexcept
        : m_rep( static_cast< RepresentType >( value ) ) {}

    template < typename Rep2, typename Period2 >
    requires( std::is_floating_point_v< RepresentType > or
              ( RatioDivide< Period2, Period >::Denominator == 1 and not std::is_floating_point_v< Rep2 > ) )
    constexpr explicit Quantity( const Quantity< Rep2, Period2 >& quantity ) noexcept
        : m_rep( quantityCast< Quantity >( quantity ).count() ) {}

    ONYX_NO_DISCARD constexpr auto count() const noexcept -> RepresentType { return m_rep; }

    ONYX_NO_DISCARD constexpr auto operator+() const noexcept -> std::common_type_t< Quantity > {
        return std::common_type_t< Quantity >( *this );
    }

    ONYX_NO_DISCARD constexpr auto operator-() const noexcept -> std::common_type_t< Quantity > {
        return std::common_type_t< Quantity >( -m_rep );
    }

    constexpr Quantity& operator++() noexcept {
        ++m_rep;
        return *this;
    }

    constexpr Quantity operator++( int ) noexcept { return Quantity( m_rep++ ); }

    constexpr Quantity& operator--() noexcept {
        --m_rep;
        return *this;
    }

    constexpr Quantity operator--( int ) noexcept { return Quantity( m_rep-- ); }

    constexpr Quantity& operator+=( const Quantity& other ) noexcept {
        m_rep += other.m_rep;
        return *this;
    }

    constexpr Quantity& operator-=( const Quantity& other ) noexcept {
        m_rep -= other.m_rep;
        return *this;
    }

    constexpr Quantity& operator*=( const RepresentType& other ) noexcept {
        m_rep *= other;
        return *this;
    }

    friend constexpr auto operator/( const Quantity& first, const RepresentType& second ) noexcept -> Quantity {
        return Quantity( first.m_rep / second );
    }

    friend constexpr auto operator/( const Quantity& first, const Quantity& second ) noexcept -> RepresentType {
        return first.m_rep / second.m_rep;
    }

    constexpr Quantity& operator/=( const RepresentType& other ) noexcept {
        m_rep /= other;
        return *this;
    }

    constexpr Quantity& operator%=( const RepresentType& other ) noexcept {
        m_rep %= other;
        return *this;
    }

    constexpr Quantity& operator%=( const Quantity& other ) noexcept {
        m_rep %= other.count();
        return *this;
    }
    friend constexpr auto operator==( const Quantity& first, const Quantity& second ) noexcept -> bool {
        return first.m_rep == second.m_rep;
    }
    friend constexpr auto operator<=>( const Quantity& first, const Quantity& second ) noexcept -> decltype( auto ) {
        return first.m_rep <=> second.m_rep;
    }
    ONYX_NO_DISCARD static constexpr Quantity zero() noexcept { return Quantity( RepresentType( 0 ) ); }

    ONYX_NO_DISCARD static constexpr Quantity min() noexcept {
        return Quantity( std::numeric_limits< RepresentType >::min() );
    }

    ONYX_NO_DISCARD static constexpr Quantity max() noexcept {
        return Quantity( std::numeric_limits< RepresentType >::max() );
    }

  private:
    RepresentType m_rep;
};

template < StorageQuantity TTo, StorageQuantity TFrom >
ONYX_NO_DISCARD constexpr auto quantityCast( const TFrom& quantity ) noexcept -> TTo {
    // convert Quantity to another Quantity; truncate
    using CalRatioType = RatioDivide< typename TFrom::PeriodType, typename TTo::PeriodType >;
    using CommonRepType = std::common_type_t< typename TTo::RepresentType, typename TFrom::RepresentType, uint64_t >;
    using ToRepresentType = typename TTo::RepresentType;

    constexpr bool NumIsOne = CalRatioType::Numerator == 1;
    constexpr bool DenIsOne = CalRatioType::Denominator == 1;

    if constexpr( DenIsOne ) {
        if constexpr( NumIsOne ) {
            return static_cast< TTo >( static_cast< ToRepresentType >( quantity.count() ) );
        } else {
            return static_cast< TTo >(
                static_cast< ToRepresentType >( static_cast< CommonRepType >( quantity.count() ) *
                                                static_cast< CommonRepType >( CalRatioType::Numerator ) ) );
        }
    } else {
        if constexpr( NumIsOne ) {
            return static_cast< TTo >(
                static_cast< ToRepresentType >( static_cast< CommonRepType >( quantity.count() ) /
                                                static_cast< CommonRepType >( CalRatioType::Denominator ) ) );
        } else {
            return static_cast< TTo >(
                static_cast< ToRepresentType >( static_cast< CommonRepType >( quantity.count() ) *
                                                static_cast< CommonRepType >( CalRatioType::Numerator ) /
                                                static_cast< CommonRepType >( CalRatioType::Denominator ) ) );
        }
    }
}

template < typename ToPeriod, typename FromPeriod, typename Rep >
requires IsRatio< ToPeriod > && IsRatio< FromPeriod > && std::is_arithmetic_v< Rep >
ONYX_NO_DISCARD constexpr auto quantityCast( Rep value ) noexcept -> Rep {
    using FromQuantity = Quantity< Rep, FromPeriod >;
    using ToQuantity = Quantity< Rep, ToPeriod >;

    return quantityCast< ToQuantity >( FromQuantity{ value } ).count();
}

template < typename ToPeriod, typename FromPeriod, typename VectorT >
requires IsRatio< ToPeriod > && IsRatio< FromPeriod > && IsVector< VectorT >
ONYX_NO_DISCARD constexpr auto quantityCast( VectorT value ) noexcept -> VectorT {
    using FromQuantity = Quantity< typename VectorT::ScalarT, FromPeriod >;
    using ToQuantity = Quantity< typename VectorT::ScalarT, ToPeriod >;

    if constexpr( IsVector2< VectorT > ) {
        return VectorT{ quantityCast< ToQuantity >( FromQuantity{ value.X } ).count(),
                        quantityCast< ToQuantity >( FromQuantity{ value.Y } ).count() };
    } else if constexpr( IsVector3< VectorT > ) {
        return VectorT{ quantityCast< ToQuantity >( FromQuantity{ value.X } ).count(),
                        quantityCast< ToQuantity >( FromQuantity{ value.Y } ).count(),
                        quantityCast< ToQuantity >( FromQuantity{ value.Z } ).count() };
    } else {
        return VectorT{ quantityCast< ToQuantity >( FromQuantity{ value.X } ).count(),
                        quantityCast< ToQuantity >( FromQuantity{ value.Y } ).count(),
                        quantityCast< ToQuantity >( FromQuantity{ value.Z } ).count(),
                        quantityCast< ToQuantity >( FromQuantity{ value.W } ).count() };
    }
}

template < typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2 >
ONYX_NO_DISCARD constexpr auto operator+( const Quantity< TRep1, TPeriod1 >& first,
                                          const Quantity< TRep2, TPeriod2 >& second ) noexcept
    -> std::common_type_t< Quantity< TRep1, TPeriod1 >, Quantity< TRep2, TPeriod2 > > {
    using ResultType = std::common_type_t< Quantity< TRep1, TPeriod1 >, Quantity< TRep2, TPeriod2 > >;
    return ResultType( ResultType( first ).count() + ResultType( second ).count() );
}
template < typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2 >
[[nodiscard]] constexpr auto operator-( const Quantity< TRep1, TPeriod1 >& first,
                                        const Quantity< TRep2, TPeriod2 >& second ) noexcept
    -> std::common_type_t< Quantity< TRep1, TPeriod1 >, Quantity< TRep2, TPeriod2 > > {
    using ResultType = std::common_type_t< Quantity< TRep1, TPeriod1 >, Quantity< TRep2, TPeriod2 > >;
    return ResultType( ResultType( first ).count() - ResultType( second ).count() );
}

template < typename TRep, typename TPeriod >
ONYX_NO_DISCARD constexpr auto clamp( const Quantity< TRep, TPeriod >& quantity,
                                      const Quantity< TRep, TPeriod >& min,
                                      const Quantity< TRep, TPeriod >& max ) noexcept -> Quantity< TRep, TPeriod > {
    if( quantity.count() < min.count() )
        return min;
    else if( quantity.count() > max.count() )
        return max;
    else
        return quantity;
}
} // namespace onyx
