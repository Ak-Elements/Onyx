#pragma once
#include <onyx/units/ratio.h>

namespace onyx
{
    template<typename T>
    concept StorageQuantity = requires(const T & obj)
    {
        typename T::RepresentType;
        typename T::PeriodType;
            requires(Numeric<typename T::RepresentType>);
        { obj.Count() } -> std::same_as<typename T::RepresentType>;
    };

    template<Numeric Rep, typename Period>
    struct Quantity;

    template<StorageQuantity TTo, StorageQuantity TFrom>
    [[nodiscard]] constexpr auto QuantityCast(const TFrom& Quantity) noexcept -> TTo;


    template<Numeric Rep, typename Period>
    struct Quantity
    {
    public:
        using RepresentType = Rep;
        using PeriodType = typename Period::Type;

        constexpr Quantity() = default;

        template<typename Rep2>
            requires(std::convertible_to<const Rep2&, RepresentType> and (std::is_floating_point_v<RepresentType> or not std::is_floating_point_v<Rep2>))
        constexpr explicit Quantity(const Rep2& value) noexcept
            : rep_(static_cast<RepresentType>(value))
        {
        }

        template<typename Rep2, typename Period2>
            requires(
        std::is_floating_point_v<RepresentType> or (RatioDivide<Period2, Period>::Denominator == 1 and not std::is_floating_point_v<Rep2>))
            constexpr explicit Quantity(const Quantity<Rep2, Period2>& Quantity) noexcept
            : rep_(QuantityCast<Quantity>(Quantity).Count())
        {
        }

        [[nodiscard]] constexpr auto Count() const noexcept -> RepresentType { return rep_; }

        [[nodiscard]] constexpr auto operator+() const noexcept -> std::common_type_t<Quantity>
        {
            return std::common_type_t<Quantity>(*this);
        }

        [[nodiscard]] constexpr auto operator-() const noexcept -> std::common_type_t<Quantity>
        {
            return std::common_type_t<Quantity>(-rep_);
        }

        constexpr Quantity& operator++() noexcept
        {
            ++rep_;
            return *this;
        }

        constexpr Quantity operator++(int) noexcept { return Quantity(rep_++); }

        constexpr Quantity& operator--() noexcept
        {
            --rep_;
            return *this;
        }

        constexpr Quantity operator--(int) noexcept { return Quantity(rep_--); }

        constexpr Quantity& operator+=(const Quantity& other) noexcept
        {
            rep_ += other.rep_;
            return *this;
        }

        constexpr Quantity& operator-=(const Quantity& other) noexcept
        {
            rep_ -= other.rep_;
            return *this;
        }

        constexpr Quantity& operator*=(const RepresentType& other) noexcept
        {
            rep_ *= other;
            return *this;
        }

        friend constexpr auto operator/(const Quantity& first, const RepresentType& second) noexcept -> Quantity
        {
            return Quantity(first.rep_ / second);
        }

        friend constexpr auto operator/(const Quantity& first, const Quantity& second) noexcept -> RepresentType
        {
            return first.rep_ / second.rep_;
        }

        constexpr Quantity& operator/=(const RepresentType& other) noexcept
        {
            rep_ /= other;
            return *this;
        }

        constexpr Quantity& operator%=(const RepresentType& other) noexcept
        {
            rep_ %= other;
            return *this;
        }

        constexpr Quantity& operator%=(const Quantity& other) noexcept
        {
            rep_ %= other.Count();
            return *this;
        }
        friend constexpr auto operator==(const Quantity& first, const Quantity& second) noexcept -> bool
        {
            return first.rep_ == second.rep_;
        }
        friend constexpr auto operator<=>(const Quantity& first, const Quantity& second) noexcept -> decltype(auto)
        {
            return first.rep_ <=> second.rep_;
        }
        [[nodiscard]] static constexpr Quantity Zero() noexcept { return Quantity(RepresentType(0)); }

        [[nodiscard]] static constexpr Quantity Min() noexcept { return Quantity(std::numeric_limits<RepresentType>::min()); }

        [[nodiscard]] static constexpr Quantity Max() noexcept { return Quantity(std::numeric_limits<RepresentType>::max()); }

    private:
        RepresentType rep_;
    };

    template<StorageQuantity TTo, StorageQuantity TFrom>
    [[nodiscard]] constexpr auto QuantityCast(const TFrom& quantity) noexcept -> TTo
    {
        // convert Quantity to another Quantity; truncate
        using CalRatioType = RatioDivide<typename TFrom::PeriodType, typename TTo::PeriodType>;
        using CommonRepType = std::common_type_t<typename TTo::RepresentType, typename TFrom::RepresentType, onyxU64>;
        using ToRepresentType = typename TTo::RepresentType;

        constexpr bool numIsOne = CalRatioType::Numerator == 1;
        constexpr bool denIsOne = CalRatioType::Denominator == 1;

        if constexpr (denIsOne)
        {
            if constexpr (numIsOne)
            {
                return static_cast<TTo>(static_cast<ToRepresentType>(quantity.Count()));
            }
            else
            {
                return static_cast<TTo>(static_cast<ToRepresentType>(
                    static_cast<CommonRepType>(quantity.Count()) * static_cast<CommonRepType>(CalRatioType::Numerator)));
            }
        }
        else
        {
            if constexpr (numIsOne)
            {
                return static_cast<TTo>(static_cast<ToRepresentType>(
                    static_cast<CommonRepType>(quantity.Count()) / static_cast<CommonRepType>(CalRatioType::Denominator)));
            }
            else
            {
                return static_cast<TTo>(static_cast<ToRepresentType>(
                    static_cast<CommonRepType>(quantity.Count()) * static_cast<CommonRepType>(CalRatioType::Numerator)
                    / static_cast<CommonRepType>(CalRatioType::Denominator)));
            }
        }
    }

    template<typename ToPeriod, typename FromPeriod, typename Rep> requires IsRatio<ToPeriod> && IsRatio<FromPeriod> && std::is_arithmetic_v<Rep>
    [[nodiscard]] constexpr auto QuantityCast(Rep value) noexcept -> Rep
    {
        using FromQuantity = Quantity<Rep, FromPeriod>;
        using ToQuantity = Quantity<Rep, ToPeriod>;

        return QuantityCast<ToQuantity>(FromQuantity{ value }).Count();
    }

    template<typename ToPeriod, typename FromPeriod, typename VectorT> requires IsRatio<ToPeriod>&& IsRatio<FromPeriod> && IsVector<VectorT>
    [[nodiscard]] constexpr auto QuantityCast(VectorT value) noexcept -> VectorT
    {
        using FromQuantity = Quantity<typename VectorT::ScalarT, FromPeriod>;
        using ToQuantity = Quantity<typename VectorT::ScalarT, ToPeriod>;

        if constexpr (IsVector2<VectorT>)
        {
            return VectorT
            {
                QuantityCast<ToQuantity>(FromQuantity{ value.X }).Count(),
                QuantityCast<ToQuantity>(FromQuantity{ value.Y }).Count()
            };
        }
        else if constexpr (IsVector3<VectorT>)
        {
            return VectorT
            {
                QuantityCast<ToQuantity>(FromQuantity{ value.X }).Count(),
                QuantityCast<ToQuantity>(FromQuantity{ value.Y }).Count(),
                QuantityCast<ToQuantity>(FromQuantity{ value.Z }).Count()
            };
        }
        else
        {
            return VectorT
            {
                QuantityCast<ToQuantity>(FromQuantity{ value.X }).Count(),
                QuantityCast<ToQuantity>(FromQuantity{ value.Y }).Count(),
                QuantityCast<ToQuantity>(FromQuantity{ value.Z }).Count(),
                QuantityCast<ToQuantity>(FromQuantity{ value.W }).Count()
            };
        }
    }

    template<typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
    [[nodiscard]] constexpr auto
        operator+(const Quantity<TRep1, TPeriod1>& first, const Quantity<TRep2, TPeriod2>& second) noexcept
        -> std::common_type_t<Quantity<TRep1, TPeriod1>, Quantity<TRep2, TPeriod2>>
    {
        using ResultType = std::common_type_t<Quantity<TRep1, TPeriod1>, Quantity<TRep2, TPeriod2>>;
        return ResultType(ResultType(first).Count() + ResultType(second).Count());
    }
    template<typename TRep1, typename TPeriod1, typename TRep2, typename TPeriod2>
    [[nodiscard]] constexpr auto
        operator-(const Quantity<TRep1, TPeriod1>& first, const Quantity<TRep2, TPeriod2>& second) noexcept
        -> std::common_type_t<Quantity<TRep1, TPeriod1>, Quantity<TRep2, TPeriod2>>
    {
        using ResultType = std::common_type_t<Quantity<TRep1, TPeriod1>, Quantity<TRep2, TPeriod2>>;
        return ResultType(ResultType(first).Count() - ResultType(second).Count());
    }

    template<typename TRep, typename TPeriod>
    [[nodiscard]] constexpr auto Clamp(
        const Quantity<TRep, TPeriod>& quantity,
        const Quantity<TRep, TPeriod>& min,
        const Quantity<TRep, TPeriod>& max) noexcept -> Quantity<TRep, TPeriod>
    {
        if (quantity.Count() < min.Count())
            return min;
        else if (quantity.Count() > max.Count())
            return max;
        else
            return quantity;
    }
}