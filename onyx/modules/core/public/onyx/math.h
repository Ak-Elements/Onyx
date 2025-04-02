#pragma once

namespace Onyx
{
    template <typename Scalar>
    constexpr bool IsEqual(Scalar lhs, Scalar rhs)
    {
        if constexpr (std::is_floating_point_v<Scalar>)
        {
            constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();
            return std::abs(lhs - rhs) <= epsilon;
        }
        else
            return lhs == rhs;
    }

    template <typename Scalar, typename = std::enable_if_t<std::is_floating_point_v<Scalar>>>
    constexpr bool IsEqual(Scalar lhs, Scalar rhs, Scalar epsilon)
    {
        return std::abs(lhs - rhs) <= epsilon;
    }

    template <typename Scalar>
    constexpr bool IsZero(Scalar value)
    {
        if constexpr (std::is_floating_point_v<Scalar>)
        {
            constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();
            return (value >= -epsilon) && (value <= epsilon);
        }
        else
            return value == 0;
    }

    template <typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
    constexpr bool IsZero(Scalar value, Scalar epsilon)
    {
        return (value >= -epsilon) && (value <= epsilon);
    }

    template <typename ToT, typename FromT>
    constexpr ToT numeric_cast(FromT value)
    {
        ONYX_ASSERT(value >= std::numeric_limits<ToT>::lowest() && value <= std::numeric_limits<ToT>::max());
        return static_cast<ToT>(value);
    }

#if !ONYX_IS_FINAL
    template <typename Scalar>
    constexpr bool IsAdditionSafe(Scalar lhs, Scalar rhs)
    {
        const bool isOverflow = (rhs > 0) && (lhs > (std::numeric_limits<Scalar>::max() - rhs));

        if constexpr (std::is_signed_v<Scalar>)
        {
            const bool isUnderflow = (rhs < 0) && (lhs < (std::numeric_limits<Scalar>::lowest() - rhs));
            return (isOverflow == false) && (isUnderflow == false);
        }
        else
        {
            return (isOverflow == false);
        }
    }

    template <typename Scalar>
    constexpr bool IsSubtractionSafe(Scalar lhs, Scalar rhs)
    {
        const Scalar min = std::numeric_limits<Scalar>::lowest();
       
        const bool isUnderflow = (rhs > 0) && (lhs < (min + rhs));

        if constexpr (std::is_signed_v<Scalar>)
        {
            const Scalar max = std::numeric_limits<Scalar>::max();
            const bool isOverflow = (rhs < 0) && (lhs > (max + rhs));
            return (isOverflow == false) && (isUnderflow == false);
        }
        else
        {
            return (isUnderflow == false);
        }
    }

    template <typename Scalar>
    constexpr bool IsMultiplicationSafe(Scalar lhs, Scalar rhs)
    {
        constexpr Scalar min = std::numeric_limits<Scalar>::lowest();

        if constexpr (std::is_signed_v<Scalar>)
        {
            if (IsEqual(lhs, Scalar(-1)) && (rhs == min))
                return false;

            if (IsEqual(rhs, Scalar(-1)) && (lhs == min))
                return false;
        }
        
        const bool isOverflow = (rhs != 0) && (lhs > (std::numeric_limits<Scalar>::max() / rhs));
        const bool isUnderflow = (rhs != 0) && (lhs < (std::numeric_limits<Scalar>::lowest() / rhs));

        return (isOverflow == false) && (isUnderflow == false);
    }

    template <typename Scalar>
    constexpr bool IsDivisionSafe(Scalar lhs, Scalar rhs)
    {
        if constexpr (std::is_signed_v<Scalar> == false)
        {
            if (IsEqual(rhs, Scalar(-1)) && (lhs == std::numeric_limits<Scalar>::lowest()))
                return false;
        }

        return true;
    }
#endif
}