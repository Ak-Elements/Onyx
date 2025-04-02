#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <randomnumbertuplegenerator.h>

namespace Onyx::Geometry
{
    TEMPLATE_TEST_CASE("Vector2 Constructor", "[Vector2][Constructor][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64, Vector2u8, Vector2u16, Vector2u32, Vector2u64)
    {
        GIVEN("a default contstructed vector")
        {
            TestType vec;
            THEN("X and Y are zero")
            {
                REQUIRE(IsZero(vec[0]));
                REQUIRE(IsZero(vec[1]));
            }

            WHEN("constructed with a single scalar")
            {
                typename TestType::ScalarT scalar = static_cast<typename TestType::ScalarT>(2.0);
                vec = TestType(scalar);
                THEN("X and Y equal to the scalar ")
                {
                    REQUIRE(IsEqual(vec[0], scalar));
                    REQUIRE(IsEqual(vec[1], scalar));
                }
            }

            WHEN("constructed with a two scalars")
            {
                typename TestType::ScalarT x = static_cast<typename TestType::ScalarT>(2.0);
                typename TestType::ScalarT y = static_cast<typename TestType::ScalarT>(4.0);
                vec = TestType(x, y);
                THEN("X and Y equal to their respective scalar ")
                {
                    REQUIRE(IsEqual(vec[0], x));
                    REQUIRE(IsEqual(vec[1], y));
                }
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Comparision", "[Vector2][Comparision][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64, Vector2u8, Vector2u16, Vector2u32, Vector2u64)
    {
        GIVEN("two vectors a[5, 3] and b[6, 3]")
        {
            TestType a{ TestType::ScalarT(5), TestType::ScalarT(3) };
            TestType b{ TestType::ScalarT(6), TestType::ScalarT(3) };
            THEN("a == b returns false")
            {
                REQUIRE_FALSE(a == b);
            }

            THEN("a != b returns true")
            {
                REQUIRE(a != b);
            }

            THEN("a < b returns VectorCompareMask::X")
            {
                REQUIRE((a < b) == VectorCompareMask::X);
            }

            THEN("b < a returns VectorCompareMask::None")
            {
                REQUIRE((b < a) == VectorCompareMask::None);
            }

            THEN("a > b returns VectorCompareMask::None")
            {
                REQUIRE((a > b) == VectorCompareMask::None);
            }

            THEN("b > a returns VectorCompareMask::X")
            {
                REQUIRE((b > a) == VectorCompareMask::X);
            }

            THEN("a <= b returns VectorCompareMask::XY")
            {
                REQUIRE((a <= b) == VectorCompareMask::XY);
            }

            THEN("b <= a returns VectorCompareMask::Y")
            {
                REQUIRE((b <= a) == VectorCompareMask::Y);
            }

            THEN("a >= b returns VectorCompareMask::Y")
            {
                REQUIRE((a >= b) == VectorCompareMask::Y);
            }

            THEN("b >= a returns VectorCompareMask::XY")
            {
                REQUIRE((b >= a) == VectorCompareMask::XY);
            }
        }

        GIVEN("two vectors a[3, 5] and b[6, 3]")
        {
            TestType a{ TestType::ScalarT(3), TestType::ScalarT(5) };
            TestType b{ TestType::ScalarT(6), TestType::ScalarT(3) };
            THEN("a == b returns false")
            {
                REQUIRE_FALSE(a == b);
            }

            THEN("a != b returns true")
            {
                REQUIRE(a != b);
            }

            THEN("a < b returns VectorCompareMask::X")
            {
                REQUIRE((a < b) == VectorCompareMask::X);
            }

            THEN("b < a returns VectorCompareMask::Y")
            {
                REQUIRE((b < a) == VectorCompareMask::Y);
            }

            THEN("a > b returns VectorCompareMask::Y")
            {
                REQUIRE((a > b) == VectorCompareMask::Y);
            }

            THEN("b > a returns VectorCompareMask::X")
            {
                REQUIRE((b > a) == VectorCompareMask::X);
            }

            THEN("a <= b returns VectorCompareMask::X")
            {
                REQUIRE((a <= b) == VectorCompareMask::X);
            }

            THEN("b <= a returns VectorCompareMask::Y")
            {
                REQUIRE((b <= a) == VectorCompareMask::Y);
            }

            THEN("a >= b returns VectorCompareMask::Y")
            {
                REQUIRE((a >= b) == VectorCompareMask::Y);
            }

            THEN("b >= a returns VectorCompareMask::X")
            {
                REQUIRE((b >= a) == VectorCompareMask::X);
            }
        }

        GIVEN("two vectors a[3, 3] and b[6, 6]")
        {
            TestType a{ TestType::ScalarT(3), TestType::ScalarT(3) };
            TestType b{ TestType::ScalarT(6), TestType::ScalarT(6) };

            THEN("a == b returns false")
            {
                REQUIRE_FALSE(a == b);
            }

            THEN("a != b returns true")
            {
                REQUIRE(a != b);
            }

            THEN("a < b returns VectorCompareMask::XY")
            {
                REQUIRE((a < b) == VectorCompareMask::XY);
            }

            THEN("b < a returns VectorCompareMask::None")
            {
                REQUIRE((b < a) == VectorCompareMask::None);
            }

            THEN("a > b returns VectorCompareMask::None")
            {
                REQUIRE((a > b) == VectorCompareMask::None);
            }

            THEN("b > a returns VectorCompareMask::XY")
            {
                REQUIRE((b > a) == VectorCompareMask::XY);
            }

            THEN("a <= b returns VectorCompareMask::XY")
            {
                REQUIRE((a <= b) == VectorCompareMask::XY);
            }

            THEN("b <= a returns VectorCompareMask::None")
            {
                REQUIRE((b <= a) == VectorCompareMask::None);
            }

            THEN("a >= b returns VectorCompareMask::None")
            {
                REQUIRE((a >= b) == VectorCompareMask::None);
            }

            THEN("b >= a returns VectorCompareMask::XY")
            {
                REQUIRE((b >= a) == VectorCompareMask::XY);
            }
        }

        GIVEN("two vectors a[3, 3] and b[3, 3]")
        {
            TestType a{ TestType::ScalarT(3), TestType::ScalarT(3) };
            TestType b{ TestType::ScalarT(3), TestType::ScalarT(3) };

            THEN("a == b returns true")
            {
                REQUIRE(a == b);
            }

            THEN("a != b returns false")
            {
                REQUIRE_FALSE(a != b);
            }
        }
    }
   
    TEMPLATE_TEST_CASE("Vector2 Arithmetic", "[Vector2][Arithmetic][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64)
    {
        GIVEN("a [1, 0] unit vector")
        {
            TestType vec = TestType::X_Unit();
            WHEN("adding a scalar of 2")
            {
                typename TestType::ScalarT scalar = static_cast<typename TestType::ScalarT>(2.0);
                vec += scalar;
                THEN("The vector is [3,2]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(3)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                }
            }

            WHEN("adding a Vector2[1,2]")
            {
                typename TestType::ScalarT x = static_cast<typename TestType::ScalarT>(1.0);
                typename TestType::ScalarT y = static_cast<typename TestType::ScalarT>(2.0);
                vec += TestType(x, y);
                THEN("The vector is [2,2]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(2)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                }
            }

            WHEN("subtracting a Vector2[4,3]")
            {
                typename TestType::ScalarT x = static_cast<typename TestType::ScalarT>(4.0);
                typename TestType::ScalarT y = static_cast<typename TestType::ScalarT>(3.0);
                vec -= TestType(x, y);
                THEN("The vector is [-3,-3]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(-3)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(-3)));
                }
            }

            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("The vector is [2,0]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(2)));
                    REQUIRE(IsZero(vec[1]));
                }
            }
        }

        GIVEN("a [0, 1] unit vector")
        {
            TestType vec = TestType::Y_Unit();
            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("the vector is [0,2]")
                {
                    REQUIRE(IsZero(vec[0]));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                }
            }
        }

        GIVEN("a [1, 1] vector")
        {
            TestType vec { TestType::ScalarT(1) };
            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("the vector is [2,2]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(2)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                }
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Dot Product", "[Vector2][DotProduct][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64, Vector2u8, Vector2u16, Vector2u32, Vector2u64)
    {
        GIVEN("2 Vectors a[1, 0] and b[0, 1]")
        {
            TestType a = TestType::X_Unit();
            TestType b = TestType::Y_Unit();
            THEN("a . b equals zero")
            {
                REQUIRE(a.Dot(b) == TestType::ScalarT(0));
            }

            THEN("b . a equals zero")
            {
                REQUIRE(b.Dot(a) == TestType::ScalarT(0));
            }

            THEN("a . a equals 1")
            {
                REQUIRE(a.Dot(a) == TestType::ScalarT(1));
            }

            if constexpr (std::is_floating_point_v<typename TestType::ScalarT> || std::is_signed_v<typename TestType::ScalarT>)
            {
                THEN("a . -a equals 1")
                {
                    REQUIRE(a.Dot(-a) == TestType::ScalarT(-1));
                }
            }
        }

        GIVEN("2 Vectors a[2, 3] and b[5, 7] vector")
        {
            TestType a { TestType::ScalarT(2), TestType::ScalarT(3) };
            TestType b { TestType::ScalarT(5), TestType::ScalarT(7) };
            THEN("a . b equals 31")
            {
                REQUIRE(a.Dot(b) == 31);
            }

            THEN("b . a equals 31")
            {
                REQUIRE(b.Dot(a) == 31);
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Normalize", "[Vector2][Normalize][template]", Vector2f, Vector2d)
    {
        GIVEN("a vector a[4, 3]")
        {
            TestType a{ TestType::ScalarT(4), TestType::ScalarT(3) };
            const auto length = a.Length();
            TestType result = a / length;
            THEN("a.Normalized() returns a unit length vector [0.8, 0.6]")
            {
                TestType normalized = a.Normalized();
                REQUIRE(normalized == result);
            }

            THEN("a.Normalize() changes a to the unit length vector [0.8, 0.6]")
            {
                a.Normalize();
                REQUIRE(a == result);
            }
        }

        GIVEN("a vector a[5, 3]")
        {
            TestType a { TestType::ScalarT(5), TestType::ScalarT(3) };
            const auto length = a.Length();
            TestType result = a / length;
            THEN("a.Normalized() returns a unit length vector [0.857493, 0.514496]")
            {
                TestType normalized = a.Normalized();
                REQUIRE(normalized == result);
            }

            THEN("a.Normalize() changes a to the unit length vector [0.857493, 0.514496]")
            {
                a.Normalize();
                REQUIRE(a == result);
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Normalize", "[Vector2][Normalize][template]", Vector2s8, Vector2s16, Vector2s32, Vector2u8, Vector2u16, Vector2u32)
    {
        GIVEN("a vector a[4, 3]")
        {
            TestType a{ TestType::ScalarT(4), TestType::ScalarT(3) };
            const auto length = a.Length();
            Vector2f result = { a[0] / length, a[1] / length };
            THEN("a.Normalized() returns a unit length float vector [0.8, 0.6]")
            {
                Vector2f normalized = a.Normalized();
                REQUIRE(normalized == result);
            }
        }

        GIVEN("a vector a[5, 3]")
        {
            TestType a{ TestType::ScalarT(5), TestType::ScalarT(3) };
            const auto length = a.Length();
            Vector2f result = { a[0] / length, a[1] / length };
            THEN("a.Normalized() returns a unit length float vector [0.857493, 0.514496]")
            {
                Vector2f normalized = a.Normalized();
                REQUIRE(normalized == result);
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Length", "[Vector2][Length][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64, Vector2u8, Vector2u16, Vector2u32, Vector2u64)
    {
        using ScalarT = typename TestType::ScalarT;
        using FloatingPointScalarT = typename TestType::FloatingPointScalarT;
        using DotT = typename TestType::DotT;

        GIVEN("a vector a[5, 3]")
        {
            constexpr DotT squaredLength = 34;
            FloatingPointScalarT length = static_cast<FloatingPointScalarT>(std::sqrt(squaredLength));
            TestType a { ScalarT(5), ScalarT(3) };
            THEN("a.Length() returns ~5.83095")
            {
                REQUIRE(IsEqual(a.Length(), length));
            }

            THEN("a.LengthSquared() returns 34")
            {
                REQUIRE(IsEqual(a.LengthSquared(), squaredLength));
            }

            THEN("a.LengthSquared() equals a . a")
            {
                REQUIRE(IsEqual(a.LengthSquared(), a.Dot(a)));
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Rotate", "[Vector2][Rotate][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64)
    {
        using RotatedVectorScalarT = typename TestType::FloatingPointScalarT;
        using RotatedVectorT = Vector2<RotatedVectorScalarT>;

        constexpr float epsilon = []()
            {
                if constexpr (sizeof(TestType::ScalarT) <= sizeof(onyxF32))
                    return 1e-5f;
                else
                    return 1e-14f;
            }();

        auto i = GENERATE(30, 45, 60, 90, 180, 240, 270, -30, -45, -60, -90, -180, -240, -270, take(100, random(-180, 180)));

        const RotatedVectorScalarT angleRadians = static_cast<RotatedVectorScalarT>(i * std::numbers::pi_v<long double> / 180.0);
        const RotatedVectorScalarT sinAlpha = std::sin(angleRadians);
        const RotatedVectorScalarT cosAlpha = std::cos(angleRadians);

        Matrix2<RotatedVectorScalarT> rotationMatrix
        {
            cosAlpha, -sinAlpha,
            sinAlpha, cosAlpha
        };

        GIVEN("a vector [3, 5]")
        {
            TestType a{ static_cast<TestType::ScalarT>(3), static_cast<TestType::ScalarT>(5) };
            RotatedVectorT expected{ static_cast<RotatedVectorScalarT>(a[0]), static_cast<RotatedVectorScalarT>(a[1]) };
            //30°
            {
                WHEN(Format::Format("rotated by {} degrees", i))
                {
                    expected = expected * rotationMatrix;
                    const RotatedVectorT& rotated = a.Rotated(angleRadians);
                    THEN(Format::Format("a' is ~[{}, {}]", expected[0], expected[1]))
                    {
                        REQUIRE(rotated.IsEqual(expected, epsilon));
                    }
                }
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector2 Rotate To", "[Vector2][Rotate][template]", Vector2f, Vector2d, Vector2s8, Vector2s16, Vector2s32, Vector2s64)
    {
        using namespace std;
        using ScalarT = typename TestType::ScalarT;
        using RotatedVectorScalarT = typename TestType::FloatingPointScalarT;
        using RotatedVectorT = Vector2<RotatedVectorScalarT>;
        using RotatedVectorDotT = typename RotatedVectorT::DotT;

        constexpr auto epsilon = []()
            {
                if constexpr (sizeof(TestType::ScalarT) <= sizeof(onyxF32))
                    return 1e-5f;
                else
                    return 1e-14f;
            }();

        constexpr auto epsilon2 = []()
            {
                if constexpr (sizeof(TestType::ScalarT) <= sizeof(onyxF32))
                    return 1e-4f;
                else
                    return 1e-13f;
            }();

        // generate vector with 4 values for fromX, fromY, toX and toY
        // filter out 0,0 vectors as length 0 is not valid
        auto values = GENERATE_COPY(take(100, 
            filter([](std::vector<ScalarT> values) { return (values[0] != 0 || values[1] != 0) && (values[2] != 0 || values[3] != 0); },
                chunk(4, 
                    random<ScalarT>(static_cast<ScalarT>(-100), static_cast<ScalarT>(100))
                )
            )
        ));

        const ScalarT fromX = values[0];
        const ScalarT fromY = values[1];

        const ScalarT toX = values[2];
        const ScalarT toY = values[3];

        GIVEN(Format::Format("a vector [{}, {}]", (double)fromX, (double)fromY))
        {
            TestType a{ fromX, fromY };
            TestType to{ toX, toY };

            WHEN(Format::Format("rotated to [{}, {}]", (double)toX, (double)toY))
            {
                const RotatedVectorT& rotated = a.RotatedTo(to);
                const RotatedVectorT& normalizedTo = to.Normalized();
                const RotatedVectorT& expected = normalizedTo * a.Length();
                THEN(Format::Format("a . a' is ~1.0"))
                {
                    RotatedVectorDotT dot = rotated.Normalized().Dot(normalizedTo);
                    REQUIRE(IsEqual<RotatedVectorDotT>(dot, RotatedVectorDotT(1), epsilon));
                    REQUIRE(expected.IsEqual(rotated, epsilon2));
                }
            }
        }
    }
}
