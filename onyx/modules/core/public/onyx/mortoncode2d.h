#pragma once

namespace Onyx
{
    namespace MortonCode2D_Internal
    {
        constexpr onyxU32 ENCODE_MASK_32[5] = { 0x0000FFFF, 0x00FF00FF, 0x0F0F0F0F, 0x33333333, 0x55555555 };
        constexpr onyxU64 ENCODE_MASK_64[6] = { 0x00000000FFFFFFFF, 0x0000FFFF0000FFFF, 0x00FF00FF00FF00FF, 0x0F0F0F0F0F0F0F0F, 0x3333333333333333, 0x5555555555555555 };

        template <typename MortonT, typename CoordT>
        inline constexpr MortonT EncodeCoordinate(CoordT coord)
        {
            if constexpr (sizeof(MortonT) == 4)
            {
                MortonT x = static_cast<MortonT>(coord);
                x = (x | x << 16) & ENCODE_MASK_32[0];
                x = (x | x << 8) & ENCODE_MASK_32[1];
                x = (x | x << 4) & ENCODE_MASK_32[2];
                x = (x | x << 2) & ENCODE_MASK_32[3];
                x = (x | x << 1) & ENCODE_MASK_32[4];
                return x;
            }
            else
            {
                MortonT x = static_cast<MortonT>(coord);
                x = (x | x << 32) & ENCODE_MASK_64[0];
                x = (x | x << 16) & ENCODE_MASK_64[1];
                x = (x | x << 8) & ENCODE_MASK_64[2];
                x = (x | x << 4) & ENCODE_MASK_64[3];
                x = (x | x << 2) & ENCODE_MASK_64[4];
                x = (x | x << 1) & ENCODE_MASK_64[5];
                return x;
            }

        }

        template <typename MortonT, typename CoordT>
        inline constexpr CoordT DecodeCoordinate(MortonT morton)
        {
            if constexpr (sizeof(MortonT) == 4)
            {
                MortonT x = morton & ENCODE_MASK_32[4];
                x = (x ^ (x >> 1)) & ENCODE_MASK_32[3];
                x = (x ^ (x >> 2)) & ENCODE_MASK_32[2];
                x = (x ^ (x >> 4)) & ENCODE_MASK_32[1];
                x = (x ^ (x >> 8)) & ENCODE_MASK_32[0];
                return static_cast<CoordT>(x);
            }
            else
            {
                MortonT x = morton & ENCODE_MASK_64[5];
                x = (x ^ (x >> 1)) & ENCODE_MASK_64[4];
                x = (x ^ (x >> 2)) & ENCODE_MASK_64[3];
                x = (x ^ (x >> 4)) & ENCODE_MASK_64[2];
                x = (x ^ (x >> 8)) & ENCODE_MASK_64[1];
                x = (x ^ (x >> 16)) & ENCODE_MASK_64[0];
                return static_cast<CoordT>(x);
            }
        }
    }

    template <typename MortonT, typename CoordT>
    struct MortonCode2D
    {
        static_assert(std::is_integral<MortonT>(), "T needs to be integral");
        static_assert((sizeof(MortonT) == 4) || (sizeof(MortonT) == 8), "T needs to be 32 or 64 bits");

    public:
        using CoordinateT = CoordT;
        using SignedCoordinateT = typename std::make_signed<CoordT>::type;

    private:
        static constexpr MortonT X_MASK = MortonCode2D_Internal::EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-1));
        static constexpr MortonT Y_MASK = MortonCode2D_Internal::EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-1)) << 1;

        static constexpr MortonT NEGATIVE_X_OFFSET = 1 << 0;
        static constexpr MortonT NEGATIVE_Y_OFFSET = 1 << 1;

    public:
        constexpr MortonCode2D() = default;
        explicit constexpr MortonCode2D(MortonT mortonCode)
            : m_MortonCode(mortonCode)
        {
        }

        operator MortonT() const { return m_MortonCode; } // implicit conversion to MortonT

        static constexpr MortonCode2D Encode(CoordT x, CoordT y)
        {
            using namespace MortonCode2D_Internal;
            return MortonCode2D(EncodeCoordinate<MortonT, CoordT>(x) | (EncodeCoordinate<MortonT, CoordT>(y) << 1));
        }

        static constexpr void Decode(MortonT morton, CoordT& x, CoordT& y)
        {
            using namespace Onyx::MortonCode2D_Internal;
            x = DecodeCoordinate<MortonT, CoordT>(morton);
            y = DecodeCoordinate<MortonT, CoordT>(morton >> 1);
        }

        template <SignedCoordinateT dX, SignedCoordinateT dY>
        constexpr MortonCode2D GetNeighbor() const
        {
            // no opt - called to get the same morton code
            if constexpr ((dX == 0) && (dY == 0))
            {
                return *this;
            }

            // direct neighbors

            // up/down neighbor
            if constexpr ((dX == 0) && ((dY * dY) == 1))
            {
                if constexpr (dY == 1)
                {
                    return MortonCode2D(
                        (m_MortonCode & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK));
                }
                else
                {
                    return MortonCode2D(
                        (m_MortonCode & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK));
                }

            }

            // right/left neighbor
            if constexpr (((dX * dX) == 1) && (dY == 0))
            {
                if constexpr (dX == 1)
                {
                    return MortonCode2D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (m_MortonCode & Y_MASK));
                }
                else
                {
                    return MortonCode2D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (m_MortonCode & Y_MASK));
                }

            }

            // up/down & left/right neighbor
            if constexpr (((dX * dX) == 1) && ((dY * dY) == 1))
            {
                if constexpr ((dX == 1) && (dY == 1))
                {
                    return MortonCode2D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK));
                }
                else if constexpr ((dX == 1) && (dY == -1))
                {
                    return MortonCode2D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK));
                }
                else if constexpr ((dX == -1) && (dY == 1))
                {
                    return MortonCode2D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK));
                }
                else
                {
                    return MortonCode2D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK));
                }
            }

            // non direct neighbors
            if constexpr (((dX * dX) != 1) && ((dY * dY) != 1))
            {
                using namespace MortonCode2D_Internal;
                return MortonCode2D(
                    (((m_MortonCode & X_MASK) - EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-dX))) & X_MASK) |
                    (((m_MortonCode & Y_MASK) - (EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-dY)) << 1)) & Y_MASK));
            }
        }

    private:
        MortonT m_MortonCode = 0;
    };
}
