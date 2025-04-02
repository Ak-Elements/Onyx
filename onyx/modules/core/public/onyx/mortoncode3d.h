#pragma once

#include <onyx/onyx_types.h>

namespace Onyx
{
    namespace MortonCode3D_Internal
    {
        constexpr onyxU32 ENCODE_MASK_32[5] = { 0x000003FF, 0x30000FF, 0x0300F00F, 0x30C30C3, 0x9249249 };
        constexpr onyxU64 ENCODE_MASK_64[6] = { 0x1FFFFF, 0x1F00000000FFFF, 0x1F0000FF0000FF, 0x100F00F00F00F00F, 0x10C30C30C30C30C3, 0x1249249249249249 };

        template <typename MortonT, typename CoordT>
        constexpr MortonT EncodeCoordinate(CoordT coord)
        {
            if constexpr (sizeof(MortonT) == 4)
            {
                MortonT x = static_cast<MortonT>(coord) & ENCODE_MASK_32[0];
                x = (x | x << 16) & ENCODE_MASK_32[1];
                x = (x | x << 8) & ENCODE_MASK_32[2];
                x = (x | x << 4) & ENCODE_MASK_32[3];
                x = (x | x << 2) & ENCODE_MASK_32[4];
                return x;
            }
            else
            {
                MortonT x = static_cast<MortonT>(coord) & ENCODE_MASK_64[0];
                x = (x | x << 32) & ENCODE_MASK_64[1];
                x = (x | x << 16) & ENCODE_MASK_64[2];
                x = (x | x << 8) & ENCODE_MASK_64[3];
                x = (x | x << 4) & ENCODE_MASK_64[4];
                x = (x | x << 2) & ENCODE_MASK_64[5];
                return x;
            }
        }

        template <typename MortonT, typename CoordT>
        constexpr CoordT DecodeCoordinate(MortonT morton)
        {
            if constexpr (sizeof(MortonT) == 4)
            {
                MortonT x = morton & ENCODE_MASK_32[4];
                x = (x ^ (x >> 2)) & ENCODE_MASK_32[3];
                x = (x ^ (x >> 4)) & ENCODE_MASK_32[2];
                x = (x ^ (x >> 8)) & ENCODE_MASK_32[1];
                x = (x ^ (x >> 16)) & ENCODE_MASK_32[0];
                return static_cast<CoordT>(x);
            }
            else
            {
                MortonT x = morton & ENCODE_MASK_64[5];
                x = (x ^ (x >> 2)) & ENCODE_MASK_64[4];
                x = (x ^ (x >> 4)) & ENCODE_MASK_64[3];
                x = (x ^ (x >> 8)) & ENCODE_MASK_64[2];
                x = (x ^ (x >> 16)) & ENCODE_MASK_64[1];
                x = (x ^ (x >> 32)) & ENCODE_MASK_64[0];
                return static_cast<CoordT>(x);
            }
        }
    }

    template <typename MortonT, typename CoordT>
    struct MortonCode3D
    {
        static_assert(std::is_integral<MortonT>(), "T needs to be integral");
        static_assert((sizeof(MortonT) == 4) || (sizeof(MortonT) == 8), "T needs to be 32 or 64 bits");

    public:
        using CoordinateT = CoordT;
        using SignedCoordinateT = typename std::make_signed<CoordT>::type;

    private:
        static constexpr MortonT X_MASK = MortonCode3D_Internal::EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-1));
        static constexpr MortonT Y_MASK = MortonCode3D_Internal::EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-1)) << 1;
        static constexpr MortonT Z_MASK = MortonCode3D_Internal::EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-1)) << 2;

        static constexpr MortonT NEGATIVE_X_OFFSET = 1 << 0;
        static constexpr MortonT NEGATIVE_Y_OFFSET = 1 << 1;
        static constexpr MortonT NEGATIVE_Z_OFFSET = 1 << 2;

    public:
        constexpr MortonCode3D() = default;
        explicit constexpr MortonCode3D(MortonT mortonCode)
            : m_MortonCode(mortonCode)
        {}

        operator MortonT() const { return m_MortonCode; } // implicit conversion to MortonT

        static constexpr MortonCode3D Encode(CoordT x, CoordT y, CoordT z)
        {
            using namespace MortonCode3D_Internal;
            return MortonCode3D(EncodeCoordinate<MortonT, CoordT>(x) | (EncodeCoordinate<MortonT, CoordT>(y) << 1) | (EncodeCoordinate<MortonT, CoordT>(z) << 2));
        }

        static constexpr void Decode(MortonT morton, CoordT& x, CoordT& y, CoordT& z)
        {
            using namespace Onyx::MortonCode3D_Internal;
            x = DecodeCoordinate<MortonT, CoordT>(morton);
            y = DecodeCoordinate<MortonT, CoordT>(morton >> 1);
            z = DecodeCoordinate<MortonT, CoordT>(morton >> 2);
        }

        template <SignedCoordinateT dX, SignedCoordinateT dY, SignedCoordinateT dZ>
        constexpr MortonCode3D GetNeighbor() const
        {
            // no opt - called to get the same morton code
            if constexpr ((dX == 0) && (dY == 0) && (dZ == 0))
            {
                return *this;
            }

            // direct neighbors
            // front/back neighbor
            if constexpr ((dX == 0) && (dY == 0) && ((dZ * dZ) == 1))
            {
                if constexpr (dZ == 1)
                {
                    return MortonCode3D(
                        (m_MortonCode & ~Z_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (m_MortonCode & ~Z_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
            }

            // up/down neighbor
            if constexpr ((dX == 0) && ((dY * dY) == 1) && (dZ == 0))
            {
                if constexpr (dY == 1)
                {
                    return MortonCode3D(
                        (m_MortonCode & ~Y_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (m_MortonCode & ~Y_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK));
                }

            }

            // right/left neighbor
            if constexpr (((dX * dX) == 1) && (dY == 0) && (dZ == 0))
            {
                if constexpr (dX == 1)
                {
                    return MortonCode3D(
                        (m_MortonCode & ~X_MASK) |
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (m_MortonCode & ~X_MASK) |
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK));
                }

            }

            // up/down & left/right neighbor
            if constexpr (((dX * dX) == 1) && ((dY * dY) == 1) && (dZ == 0))
            {
                if constexpr ((dX == 1) && (dY == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (m_MortonCode & Z_MASK));
                }
                else if constexpr ((dX == 1) && (dY == -1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (m_MortonCode & Z_MASK));
                }
                else if constexpr ((dX == -1) && (dY == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (m_MortonCode & Z_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (m_MortonCode & Z_MASK));
                }
            }

            // up/down & front/back neighbor
            if constexpr ((dX == 0) && ((dY * dY) == 1) && ((dZ * dZ) == 1))
            {
                if constexpr ((dY == 1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (m_MortonCode & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else if constexpr ((dY == 1) && (dZ == -1))
                {
                    return MortonCode3D(
                        (m_MortonCode & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
                else if constexpr ((dY == -1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (m_MortonCode & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (m_MortonCode & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
            }

            // front/back & left/right neighbor
            if constexpr (((dX * dX) == 1) && (dY == 0) && ((dZ * dZ) == 1))
            {
                if constexpr ((dX == 1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (m_MortonCode & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else if constexpr ((dX == 1) && (dZ == -1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (m_MortonCode & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
                else if constexpr ((dX == -1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (m_MortonCode & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (m_MortonCode & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
            }

            // all 3 ways
            if constexpr (((dX * dX) == 1) && ((dY * dY) == 1) && ((dZ * dZ) == 1))
            {
                if constexpr ((dX == 1) && (dY == 1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else if constexpr ((dX == 1) && (dY == 1) && (dZ == -1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
                else if constexpr ((dX == 1) && (dY == -1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else if constexpr ((dX == 1) && (dY == -1) && (dZ == -1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - X_MASK) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
                else if constexpr ((dX == -1) && (dY == 1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else if constexpr ((dX == -1) && (dY == 1) && (dZ == -1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - Y_MASK) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
                else if constexpr ((dX == -1) && (dY == -1) && (dZ == 1))
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - Z_MASK) & Z_MASK));
                }
                else
                {
                    return MortonCode3D(
                        (((m_MortonCode & X_MASK) - NEGATIVE_X_OFFSET) & X_MASK) |
                        (((m_MortonCode & Y_MASK) - NEGATIVE_Y_OFFSET) & Y_MASK) |
                        (((m_MortonCode & Z_MASK) - NEGATIVE_Z_OFFSET) & Z_MASK));
                }
            }



            // non direct neighbors
            if constexpr (((dX * dX) != 1) && ((dY * dY) != 1) && ((dZ * dZ) != 1))
            {
                using namespace MortonCode3D_Internal;
                return MortonCode3D(
                    (((m_MortonCode & X_MASK) - EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-dX))) & X_MASK) |
                    (((m_MortonCode & Y_MASK) - (EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-dY)) << 1)) & Y_MASK) |
                    (((m_MortonCode & Z_MASK) - (EncodeCoordinate<MortonT, CoordT>(static_cast<CoordT>(-dZ)) << 2)) & Z_MASK));
            }
        }

    private:
        MortonT m_MortonCode = 0;
    };
}
