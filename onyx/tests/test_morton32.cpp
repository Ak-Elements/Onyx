#include <catch2/catch_test_macros.hpp>
#include "randomnumbertuplegenerator.h"

#include <onyx/morton.h>

namespace Onyx::MortonCode
{

SCENARIO("MortonCode2D_U32", "[morton][32bit][2d]")
{
    using namespace std;
    using namespace Onyx;

    using MortonT = MortonCode2D_U32;

    GIVEN("A random morton code")
    {
        using CoordinateT = MortonT::CoordinateT;
        auto [initialX, initialY] = GENERATE(
            make_tuple<CoordinateT, CoordinateT>(numeric_limits<CoordinateT>::min(), numeric_limits<CoordinateT>::min()),
            make_tuple<CoordinateT, CoordinateT>(0, 0),
            make_tuple<CoordinateT, CoordinateT>(numeric_limits<CoordinateT>::max(), numeric_limits<CoordinateT>::max()),
            take(100, randomNumberTuple<CoordinateT>(numeric_limits<CoordinateT>::min(), numeric_limits<CoordinateT>::max())));

        CoordinateT x, y;
        MortonT morton = MortonT::Encode(initialX, initialY);
        MortonT::Decode(morton, x, y);

        REQUIRE(x == initialX);
        REQUIRE(y == initialY);

        CoordinateT neighborX, neighborY;
        WHEN("Going right 1")
        {
            MortonT rightNeighbor = morton.GetNeighbor<1, 0>();
            THEN("X changes by 1 and Y stays the same")
            {
                MortonT::Decode(rightNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == static_cast<CoordinateT>(initialX + 1));
                REQUIRE(neighborY == initialY);
            }
        }

        WHEN("Going left 1")
        {
            MortonT leftNeighbor = morton.GetNeighbor<-1, 0>();
            THEN("X changes by -1 and Y stays the same")
            {
                MortonT::Decode(leftNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == static_cast<CoordinateT>(initialX - 1));
                REQUIRE(neighborY == initialY);
            }
        }

        WHEN("Going up 1")
        {
            MortonT upNeighbor = morton.GetNeighbor<0, 1>();
            THEN("X stays the same and Y changes 1")
            {
                MortonT::Decode(upNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == static_cast<CoordinateT>(initialY + 1));
            }
        }

        WHEN("Going down 1")
        {
            MortonT downNeighbor = morton.GetNeighbor<0, -1>();
            THEN("X stays the same and Y changes by -1")
            {
                MortonT::Decode(downNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == static_cast<CoordinateT>(initialY - 1));
            }
        }

        WHEN("Going right 2")
        {
            MortonT rightNeighbor = morton.GetNeighbor<2, 0>();
            THEN("X changes by 2 and Y stays the same")
            {
                MortonT::Decode(rightNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == static_cast<CoordinateT>(initialX + 2));
                REQUIRE(neighborY == initialY);
            }
        }

        WHEN("Going left 2")
        {
            MortonT leftNeighbor = morton.GetNeighbor<-2, 0>();
            THEN("X changes by -2 and Y stays the same")
            {
                MortonT::Decode(leftNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == static_cast<CoordinateT>(initialX - 2));
                REQUIRE(neighborY == initialY);
            }
        }

        WHEN("Going up 2")
        {
            MortonT upNeighbor = morton.GetNeighbor<0, 2>();
            THEN("X stays the same and Y changes 2")
            {
                MortonT::Decode(upNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == static_cast<CoordinateT>(initialY + 2));
            }
        }

        WHEN("Going down 2")
        {
            MortonT downNeighbor = morton.GetNeighbor<0, -2>();
            THEN("X stays the same and Y changes by -2")
            {
                MortonT::Decode(downNeighbor, neighborX, neighborY);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == static_cast<CoordinateT>(initialY - 2));
            }
        }
    }
}

SCENARIO("MortonCode3D_U32", "[morton][32bit][3d]")
{
    using namespace std;
    using namespace Onyx;

    using MortonT = MortonCode3D_U32;

    GIVEN("A random morton code")
    {

        using CoordinateT = MortonT::CoordinateT;
        constexpr CoordinateT MAX_COORD = 1023;

        auto [initialX, initialY, initialZ] = GENERATE(
            make_tuple<CoordinateT, CoordinateT, CoordinateT>(0, 0, 0),
            make_tuple<CoordinateT, CoordinateT, CoordinateT>(10, 10, 10),
            make_tuple<CoordinateT, CoordinateT, CoordinateT>(1023, 1023, 1023),
            take(100, randomNumberTriple<CoordinateT>((CoordinateT)0, (CoordinateT)1023)));

        MortonT morton = MortonT::Encode(initialX, initialY, initialZ);

        CoordinateT x, y, z;
        MortonT::Decode(morton, x, y, z);

        REQUIRE(x == initialX);
        REQUIRE(y == initialY);
        REQUIRE(z == initialZ);

        CoordinateT neighborX, neighborY, neighborZ;
        WHEN("Going right 1")
        {
            MortonT rightNeighbor = morton.GetNeighbor<1, 0, 0>();
            THEN("Y/Z stay the same X changes by 1")
            {
                MortonT::Decode(rightNeighbor, neighborX, neighborY, neighborZ);
                REQUIRE(neighborX == ((initialX == MAX_COORD) ? 0 : (initialX + 1)));
                REQUIRE(neighborY == initialY);
                REQUIRE(neighborZ == initialZ);
            }
        }


        WHEN("Going left 1")
        {
            MortonT leftNeighbor = morton.GetNeighbor<-1, 0, 0>();
            THEN("Y/Z stay the same X changes by -1")
            {
                MortonT::Decode(leftNeighbor, neighborX, neighborY, neighborZ);
                REQUIRE(neighborX == ((initialX == 0) ? MAX_COORD : static_cast<CoordinateT>(initialX - 1)));
                REQUIRE(neighborY == initialY);
                REQUIRE(neighborZ == initialZ);
            }
        }

        WHEN("Going up 1")
        {
            MortonT upNeighbor = morton.GetNeighbor<0, 1, 0>();
            THEN("X/Z stay the same Y changes by 1")
            {
                MortonT::Decode(upNeighbor, neighborX, neighborY, neighborZ);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == ((initialY == MAX_COORD) ? 0 : static_cast<CoordinateT>(initialY + 1)));
                REQUIRE(neighborZ == initialZ);
            }
        }

        WHEN("Going down 1")
        {
            MortonT downNeighbor = morton.GetNeighbor<0, -1, 0>();
            THEN("X/Z stay the same Y changes by -1")
            {
                MortonT::Decode(downNeighbor, neighborX, neighborY, neighborZ);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == ((initialY == 0) ? MAX_COORD : static_cast<CoordinateT>(initialY - 1)));
                REQUIRE(neighborZ == initialZ);
            }
        }

        WHEN("Going forward 1")
        {
            MortonT upNeighbor = morton.GetNeighbor<0, 0, 1>();
            THEN("X/Y stay the same Z changes by 1")
            {
                MortonT::Decode(upNeighbor, neighborX, neighborY, neighborZ);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == initialY);
                REQUIRE(neighborZ == ((initialZ == MAX_COORD) ? 0 : static_cast<CoordinateT>(initialZ + 1)));
            }
        }

        WHEN("Going backwards 1")
        {
            MortonT downNeighbor = morton.GetNeighbor<0, 0, -1>();
            THEN("X/Y stay the same Z changes by -1")
            {
                MortonT::Decode(downNeighbor, neighborX, neighborY, neighborZ);
                REQUIRE(neighborX == initialX);
                REQUIRE(neighborY == initialY);
                REQUIRE(neighborZ == ((initialZ == 0) ? MAX_COORD : static_cast<CoordinateT>(initialZ - 1)));
            }
        }
    }
}
}