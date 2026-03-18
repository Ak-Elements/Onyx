#pragma once

namespace onyx
{
    template <typename T>
    concept IsRatio = requires {
        // Must have the static members Numerator and Denominator
        { T::Numerator } -> std::convertible_to<onyxS64>;
        { T::Denominator } -> std::convertible_to<onyxS64>;
    };

    template<onyxS64 TNum, onyxS64 TDemon = 1>
    struct Ratio
    {
        using Type = Ratio<TNum, TDemon>;
        constexpr static inline onyxS64 Numerator = TNum;
        constexpr static inline onyxS64 Denominator = TDemon;
    };

    namespace Detail
    {
        template<typename T>
        constexpr T abs(const T& value)
        {
            return value < 0 ? -value : value;
        }

        constexpr auto gcd(onyxS64 first, onyxS64 second) -> onyxS64
        {
            if (first == 0 and second == 0)
            {
                return 1;// avoids division by 0 in ratio_less
            }

            first = Detail::abs(first);
            second = Detail::abs(second);

            while (second != 0)
            {
                const onyxS64 temp = first;
                first = second;
                second = temp % second;
            }

            return first;
        }

        template<onyxS64 TMultiplier, onyxS64 TMultiplicand>
        constexpr inline bool is_multiply_overflow = []() {
            return Detail::abs(TMultiplier)
                <= ::std::numeric_limits<onyxS64>::max() / (TMultiplicand == 0 ? 1 : Detail::abs(TMultiplicand));
            }();

        template<onyxS64 TMultiplier, onyxS64 TMultiplicand>
        struct multiple : std::integral_constant<onyxS64, TMultiplier* TMultiplicand>
        {
            static_assert(is_multiply_overflow<TMultiplier, TMultiplicand>, "integer arithmetic overflow");
        };


        template<typename TMultiplier, typename TMultiplicand>
        struct ratio_multiply
        {
            constexpr static onyxS64 greatest_common_divisor_1 = gcd(TMultiplier::Numerator, TMultiplicand::Denominator);
            constexpr static onyxS64 greatest_common_divisor_2 = gcd(TMultiplicand::Numerator, TMultiplier::Denominator);

            using numerator = Detail::multiple<
                TMultiplier::Numerator / greatest_common_divisor_1,
                TMultiplicand::Numerator / greatest_common_divisor_2>;
            using denominator = Detail::multiple<
                TMultiplier::Denominator / greatest_common_divisor_2,
                TMultiplicand::Denominator / greatest_common_divisor_1>;
            using type = Ratio<numerator::value, denominator::value>;
        };
    }// namespace Detail


    template<typename TRatio>
    using RatioInvert = Ratio<TRatio::Denominator, TRatio::Numerator>;

    template<typename TMultiplier, typename TMultiplicand>
    using RatioMultiply = Detail::ratio_multiply<TMultiplier, TMultiplicand>::type;

    template<typename TDivisor, typename TDividend>
    using RatioDivide = RatioMultiply<TDivisor, RatioInvert<TDividend>>;
}
