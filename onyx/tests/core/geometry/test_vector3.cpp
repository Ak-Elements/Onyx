#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

namespace Onyx::Geometry
{
    TEMPLATE_TEST_CASE("Vector3 Constructor", "[Vector3][Constructor][template]", Vector3f, Vector3d, Vector3s8, Vector3s16, Vector3s32, Vector3s64, Vector3u8, Vector3u16, Vector3u32, Vector3u64)
    {
        GIVEN("a default contstructed vector")
        {
            TestType vec;
            THEN("X, Y and Z are zero")
            {
                REQUIRE(IsZero(vec[0]));
                REQUIRE(IsZero(vec[1]));
                REQUIRE(IsZero(vec[2]));
            }

            WHEN("constructed with a single scalar")
            {
                typename TestType::ScalarT scalar = static_cast<typename TestType::ScalarT>(2.0);
                vec = TestType(scalar);
                THEN("X and Y equal to the scalar ")
                {
                    REQUIRE(IsEqual(vec[0],scalar));
                    REQUIRE(IsEqual(vec[1],scalar));
                    REQUIRE(IsEqual(vec[2],scalar));
                }
            }

            WHEN("constructed with a two scalars")
            {
                typename TestType::ScalarT x = static_cast<typename TestType::ScalarT>(2.0);
                typename TestType::ScalarT y = static_cast<typename TestType::ScalarT>(4.0);
                typename TestType::ScalarT z = static_cast<typename TestType::ScalarT>(6.0);
                vec = TestType(x, y, z);
                THEN("X and Y equal to their respective scalar ")
                {
                    REQUIRE(IsEqual(vec[0],x));
                    REQUIRE(IsEqual(vec[1],y));
                    REQUIRE(IsEqual(vec[2],z));
                }
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Comparision", "[Vector2][Comparision][template]", Vector3f, Vector3d, Vector3s8, Vector3s16, Vector3s32, Vector3s64, Vector3u8, Vector3u16, Vector3u32, Vector3u64)
    {
        GIVEN("two vectors a[5, 3, 3] and b[6, 3, 4]")
        {
            TestType a { TestType::ScalarT(5), TestType::ScalarT(3), TestType::ScalarT(3) };
            TestType b { TestType::ScalarT(6), TestType::ScalarT(3), TestType::ScalarT(4) };
            THEN("a == b returns false")
            {
                REQUIRE_FALSE(a == b);
            }

            THEN("a != b returns true")
            {
                REQUIRE(a != b);
            }

            THEN("a < b returns VectorCompareMask::XZ")
            {
                REQUIRE((a < b) == VectorCompareMask::XZ);
            }

            THEN("b < a returns VectorCompareMask::None")
            {
                REQUIRE((b < a) == VectorCompareMask::None);
            }

            THEN("a > b returns VectorCompareMask::None")
            {
                REQUIRE((a > b) == VectorCompareMask::None);
            }

            THEN("b > a returns VectorCompareMask::XZ")
            {
                REQUIRE((b > a) == VectorCompareMask::XZ);
            }

            THEN("a <= b returns VectorCompareMask::XYZ")
            {
                REQUIRE((a <= b) == VectorCompareMask::XYZ);
            }

            THEN("b <= a returns VectorCompareMask::Y")
            {
                REQUIRE((b <= a) == VectorCompareMask::Y);
            }

            THEN("a >= b returns VectorCompareMask::Y")
            {
                REQUIRE((a >= b) == VectorCompareMask::Y);
            }

            THEN("b >= a returns VectorCompareMask::XYZ")
            {
                REQUIRE((b >= a) == VectorCompareMask::XYZ);
            }
        }

        GIVEN("two vectors a[3, 5, 1] and b[6, 3, 1]")
        {
            TestType a { TestType::ScalarT(3), TestType::ScalarT(5), TestType::ScalarT(1) };
            TestType b { TestType::ScalarT(6), TestType::ScalarT(3), TestType::ScalarT(1) };
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

            THEN("a <= b returns VectorCompareMask::XZ")
            {
                REQUIRE((a <= b) == VectorCompareMask::XZ);
            }

            THEN("b <= a returns VectorCompareMask::YZ")
            {
                REQUIRE((b <= a) == VectorCompareMask::YZ);
            }

            THEN("a >= b returns VectorCompareMask::YZ")
            {
                REQUIRE((a >= b) == VectorCompareMask::YZ);
            }

            THEN("b >= a returns VectorCompareMask::XZ")
            {
                REQUIRE((b >= a) == VectorCompareMask::XZ);
            }
        }

        GIVEN("two vectors a[3, 3, 3] and b[6, 6, 6]")
        {
            TestType a { TestType::ScalarT(3) };
            TestType b { TestType::ScalarT(6) };

            THEN("a == b returns false")
            {
                REQUIRE_FALSE(a == b);
            }

            THEN("a != b returns true")
            {
                REQUIRE(a != b);
            }

            THEN("a < b returns VectorCompareMask::XYZ")
            {
                REQUIRE((a < b) == VectorCompareMask::XYZ);
            }

            THEN("b < a returns VectorCompareMask::None")
            {
                REQUIRE((b < a) == VectorCompareMask::None);
            }

            THEN("a > b returns VectorCompareMask::None")
            {
                REQUIRE((a > b) == VectorCompareMask::None);
            }

            THEN("b > a returns VectorCompareMask::XYZ")
            {
                REQUIRE((b > a) == VectorCompareMask::XYZ);
            }

            THEN("a <= b returns VectorCompareMask::XYZ")
            {
                REQUIRE((a <= b) == VectorCompareMask::XYZ);
            }

            THEN("b <= a returns VectorCompareMask::None")
            {
                REQUIRE((b <= a) == VectorCompareMask::None);
            }

            THEN("a >= b returns VectorCompareMask::None")
            {
                REQUIRE((a >= b) == VectorCompareMask::None);
            }

            THEN("b >= a returns VectorCompareMask::XYZ")
            {
                REQUIRE((b >= a) == VectorCompareMask::XYZ);
            }
        }

        GIVEN("two vectors a[3, 3, 3] and b[3, 3, 3]")
        {
            TestType a { TestType::ScalarT(3) };
            TestType b { TestType::ScalarT(3) };

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

    TEMPLATE_TEST_CASE("Vector3 Arithmetic", "[Vector3][Arithmetic][template]", Vector3f, Vector3d, Vector3s8, Vector3s16, Vector3s32, Vector3s64)
    {
        GIVEN("a [1, 0, 0] unit vector")
        {
            TestType vec = TestType::X_Unit();
            WHEN("adding a Vector3[1,2,4]")
            {
                typename TestType::ScalarT x = static_cast<typename TestType::ScalarT>(1.0);
                typename TestType::ScalarT y = static_cast<typename TestType::ScalarT>(2.0);
                typename TestType::ScalarT z = static_cast<typename TestType::ScalarT>(4.0);
                vec += TestType(x, y, z);
                THEN("The vector is [2,2,4]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(2)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                    REQUIRE(IsEqual(vec[2], TestType::ScalarT(4)));
                }
            }

            WHEN("subtracting a Vector2[4,3,-10]")
            {
                typename TestType::ScalarT x = static_cast<typename TestType::ScalarT>(4.0);
                typename TestType::ScalarT y = static_cast<typename TestType::ScalarT>(3.0);
                typename TestType::ScalarT z = static_cast<typename TestType::ScalarT>(-10.0);
                vec -= TestType(x, y, z);
                THEN("The vector is [-3,-3,10]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(-3)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(-3)));
                    REQUIRE(IsEqual(vec[2], TestType::ScalarT(10)));
                }
            }

            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("The vector is [2,0,0]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(2)));
                    REQUIRE(IsZero(vec[1]));
                    REQUIRE(IsZero(vec[2]));
                }
            }
        }

        GIVEN("a [0, 1, 0] unit vector")
        {
            TestType vec = TestType::Y_Unit();
            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("the vector is [0,2,0]")
                {
                    REQUIRE(IsZero(vec[0]));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                    REQUIRE(IsZero(vec[2]));
                }
            }
        }

        GIVEN("a [0, 0, 1] unit vector")
        {
            TestType vec = TestType::Z_Unit();
            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("the vector is [0,0,2]")
                {
                    REQUIRE(IsZero(vec[0]));
                    REQUIRE(IsZero(vec[1]));
                    REQUIRE(IsEqual(vec[2], TestType::ScalarT(2)));
                }
            }
        }

        GIVEN("a [1, 1, 1] vector")
        {
            TestType vec{ TestType::ScalarT(1) };
            WHEN("muliplied by a scalar of 2")
            {
                vec *= TestType::ScalarT(2);
                THEN("the vector is [2,2,2]")
                {
                    REQUIRE(IsEqual(vec[0], TestType::ScalarT(2)));
                    REQUIRE(IsEqual(vec[1], TestType::ScalarT(2)));
                    REQUIRE(IsEqual(vec[2], TestType::ScalarT(2)));
                }
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Dot Product", "[Vector3][DotProduct][template]", Vector3f, Vector3d, Vector3s8, Vector3s16, Vector3s32, Vector3s64, Vector3u8, Vector3u16, Vector3u32, Vector3u64)
    {
        GIVEN("2 Vectors a[1, 0, 0] and b[0, 1, 0]")
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

        GIVEN("2 Vectors a[2, 3, 5] and b[5, 7, 9] vector")
        {
            TestType a{ TestType::ScalarT(2), TestType::ScalarT(3),  TestType::ScalarT(5) };
            TestType b{ TestType::ScalarT(5), TestType::ScalarT(7),  TestType::ScalarT(9) };
            THEN("a . b equals 76")
            {
                REQUIRE(a.Dot(b) == 76);
            }

            THEN("b . a equals 76")
            {
                REQUIRE(b.Dot(a) == 76);
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Normalize", "[Vector2][Normalize][template]", Vector3f, Vector3d)
    {
        using ScalarT = typename TestType::ScalarT;

        GIVEN("a vector a[4, 3, 10]")
        {
            TestType a { ScalarT(4), ScalarT(3), ScalarT(10) };
            const onyxF64 length = a.Length();
            TestType result{ a / static_cast<ScalarT>(length) };
            THEN("a.Normalized() returns a unit length vector ~[0.357771, 0.268328, 0.894427]")
            {
                TestType normalized = a.Normalized();
                REQUIRE(normalized == result);
            }

            THEN("a.Normalize() changes a to the unit length vector ~[0.357771, 0.268328, 0.894427]")
            {
                a.Normalize();
                REQUIRE(a == result);
            }
        }

        GIVEN("a vector a[5, 3, 1]")
        {
            TestType a{ ScalarT(5), ScalarT(3), ScalarT(1) };
            const onyxF64 length = a.Length();
            TestType result{ a / static_cast<ScalarT>(length) };
            THEN("a.Normalized() returns a unit length vector ~[0.845154, 0.507093, 0.169031]")
            {
                TestType normalized = a.Normalized();
                REQUIRE(normalized == result);
            }

            THEN("a.Normalize() changes a to the unit length vector ~[0.845154, 0.507093, 0.169031]")
            {
                a.Normalize();
                REQUIRE(a == result);
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Normalize", "[Vector3][Normalize][template]", Vector3s8, Vector3s16, Vector3s32, Vector3u8, Vector3u16, Vector3u32)
    {
        using ScalarT = typename TestType::ScalarT;

        GIVEN("a vector a[4, 3, 10]")
        {
            TestType a{ ScalarT(4), ScalarT(3), ScalarT(10) };
            const onyxF32 length = numeric_cast<onyxF32>(a.Length());
            Vector3f result = { a[0] / length, a[1] / length, a[2] / length};
            THEN("a.Normalized() returns a unit length float vector ~[0.357771, 0.268328, 0.894427]")
            {
                Vector3f normalized = a.Normalized();
                REQUIRE(normalized == result);
            }
        }

        GIVEN("a vector a[5, 3, 1]")
        {
            TestType a { ScalarT(5), ScalarT(3), ScalarT(1) };
            const onyxF32 length = numeric_cast<onyxF32>(a.Length());
            Vector3f result = { a[0] / length, a[1] / length, a[2] / length };
            THEN("a.Normalized() returns a unit length float vector ~[0.845154, 0.507093, 0.169031]")
            {
                Vector3f normalized = a.Normalized();
                REQUIRE(normalized == result);
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Length", "[Vector3][Length][template]", Vector3f, Vector3d, Vector3s8, Vector3s16, Vector3s32, Vector3s64, Vector3u8, Vector3u16, Vector3u32, Vector3u64)
    {
        using ScalarT = typename TestType::ScalarT;
        using FloatingPointScalarT = typename TestType::FloatingPointScalarT;
        using DotT = typename TestType::DotT;

        GIVEN("a vector a[5, 3, 7]")
        {
            constexpr DotT squaredLength = 83;
            FloatingPointScalarT length = static_cast<FloatingPointScalarT>(std::sqrt(squaredLength));
            TestType a{ ScalarT(5), ScalarT(3),  ScalarT(7) };

            THEN("a.Length() returns ~9.1104")
            {
                REQUIRE(IsEqual(a.Length(), length));
            }

            THEN("a.LengthSquared() returns 83")
            {
                REQUIRE(IsEqual(a.LengthSquared(), squaredLength));
            }

            THEN("a.LengthSquared() equals a . a")
            {
                REQUIRE(IsEqual(a.LengthSquared(), a.Dot(a)));
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Rotate", "[Vector3][Rotate][template]", Vector3f, Vector3d, Vector3s8, Vector3s16, Vector3s32, Vector3s64)
    {
        using RotatedVectorScalarT = typename TestType::FloatingPointScalarT;
        using RotatedVectorT = Vector3<RotatedVectorScalarT>;

        auto i = GENERATE(30, 45, 60, 90, 180, 240, 270, -30, -45, -60, -90, -180, -240, -270, take(100, random(-180, 180)));

        const RotatedVectorScalarT angleRadians = static_cast<RotatedVectorScalarT>(i * std::numbers::pi_v<long double> / 180.0);
        const RotatedVectorScalarT sinAlpha = std::sin(angleRadians);
        const RotatedVectorScalarT cosAlpha = std::cos(angleRadians);

        // X Axis rotation
        Matrix3<RotatedVectorScalarT> rotationMatrixPitch
        {
            1,  0,          0,
            0,  cosAlpha,   -sinAlpha,
            0,  sinAlpha,   cosAlpha
        };

        // Y Axis rotation
        Matrix3<RotatedVectorScalarT> rotationMatrixYaw
        {
            cosAlpha,   0,  sinAlpha,
            0,          1,  0,
            -sinAlpha,  0,  cosAlpha
        };

        // Z Axis rotation
        Matrix3<RotatedVectorScalarT> rotationMatrixRoll
        {
            cosAlpha,   -sinAlpha,  0,
            sinAlpha,   cosAlpha,   0,
            0,          0,          1
        };

        constexpr float epsilon = []()
        {
            if constexpr (sizeof(TestType::ScalarT) <= sizeof(onyxF32))
                return 1e-5f;
            else
                return 1e-12f;
        }();

        GIVEN("a vector [3, 5, 7]")
        {
            TestType a { static_cast<TestType::ScalarT>(3), static_cast<TestType::ScalarT>(5), static_cast<TestType::ScalarT>(7) };
            RotatedVectorT expected { static_cast<RotatedVectorScalarT>(a[0]), static_cast<RotatedVectorScalarT>(a[1]), static_cast<RotatedVectorScalarT>(a[2]) };
            //30°
            {
                WHEN(Format::Format("rotated by {} degrees in YZ", i))
                {
                    expected = expected * rotationMatrixPitch;
                    const RotatedVectorT& rotated = a.RotatedAroundX(angleRadians);
                    THEN(Format::Format("a' is ~[{}, {}, {}]", expected[0], expected[1], expected[2]))
                    {
                        bool result = rotated.IsEqual(expected, epsilon);
                        if (result == false)
                            result = false;

                        REQUIRE(rotated.IsEqual(expected, epsilon));
                    }
                }

                WHEN(Format::Format("rotated by {} degrees in ZX", i))
                {
                    expected = expected * rotationMatrixYaw;
                    const RotatedVectorT& rotated = a.RotatedAroundY(angleRadians);
                    THEN(Format::Format("a' is ~[{}, {}, {}]", expected[0], expected[1], expected[2]))
                    {
                        bool result = rotated.IsEqual(expected, epsilon);
                        if (result == false)
                            result = false;

                        REQUIRE(rotated.IsEqual(expected, epsilon));
                    }
                }

                WHEN(Format::Format("rotated by {} degrees in  XY", i))
                {
                    expected = expected * rotationMatrixRoll;
                    const RotatedVectorT& rotated = a.RotatedAroundZ(angleRadians);
                    THEN(Format::Format("a' is ~[{}, {}, {}]", expected[0], expected[1], expected[2]))
                    {
                        bool result = rotated.IsEqual(expected, epsilon);
                        if (result == false)
                            result = false;

                        REQUIRE(rotated.IsEqual(expected, epsilon));
                    }
                }
            }
        }
    }

    TEMPLATE_TEST_CASE("Vector3 Rotate To", "[Vector3][Rotate][template]", Vector3f, Vector3d, Vector3s16, Vector3s32, Vector3s64)
    {
        // Vector3s8 is skipped as it can overflow very easily in the squared length
        // Probably should disable this function for s8

        using namespace std;
        using ScalarT = typename TestType::ScalarT;
        using RotatedVectorScalarT = typename TestType::FloatingPointScalarT;
        using RotatedVectorT = Vector3<RotatedVectorScalarT>;

        constexpr auto epsilon = []()
        {
            if constexpr (sizeof(ScalarT) <= sizeof(onyxF32))
                return 1e-4f;
            else
                return 1e-14f;
        }();

        constexpr auto epsilon2 = []()
        {
            if constexpr (sizeof(ScalarT) <= sizeof(onyxF32))
                return 1e-3f;
            else
                return 1e-12f;
        }();

        // generate vector with 4 values for fromX, fromY, toX and toY
        // filter out 0,0 vectors as length 0 is not valid
        auto values = GENERATE_COPY(take(100,
            filter([](std::vector<ScalarT> values) { return (values[0] != 0 || values[1] != 0 || values[2] != 0) && (values[3] != 0 || values[4] != 0 || values[5] != 0); },
                chunk(6,
                    random<ScalarT>(static_cast<ScalarT>(-100), static_cast<ScalarT>(100))
                )
            )
        ));

        const ScalarT fromX = values[0];
        const ScalarT fromY = values[1];
        const ScalarT fromZ = values[2];

        const ScalarT toX = values[3];
        const ScalarT toY = values[4];
        const ScalarT toZ = values[5];

        GIVEN(Format::Format("a vector [{}, {}, {}]", fromX, fromY, fromZ))
        {
            TestType a{ fromX, fromY, fromZ };
            TestType to{ toX, toY, toZ };

            WHEN(Format::Format("rotated to [{}, {}, {}]", toX, toY, toZ))
            {
                const RotatedVectorT& rotated = a.RotatedTo(to);
                const RotatedVectorT& normalizedTo = to.Normalized();
                const RotatedVectorT& expected = normalizedTo * numeric_cast<RotatedVectorScalarT>(a.Length());
                THEN(Format::Format("a . a' is ~1.0"))
                {
                    onyxF64 dot = rotated.Normalized().Dot(normalizedTo);
                    REQUIRE(IsEqual<onyxF64>(dot, 1.0, epsilon));
                    REQUIRE(expected.IsEqual(rotated, epsilon2));
                }
            }
        }
    }
}
