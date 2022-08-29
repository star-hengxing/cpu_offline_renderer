#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <raytracing/Scene/scene_payload.hpp>

TEST_CASE("test the scene file loader")
{
    scene_payload payload;
    REQUIRE(payload.load("asset/cornell_box.json"));

    SUBCASE("Matrix")
    {
        constexpr auto count = 15;
        CHECK(payload.matrixes.size == count);
        CHECK(payload.matrixes.max_size == count);
    }

    SUBCASE("Shape")
    {
        constexpr auto count = 8;
        CHECK(payload.shapes.size == count);
        CHECK(payload.shapes.max_size == count);
    }

    SUBCASE("Material")
    {
        constexpr auto count = 3;
        CHECK(payload.materials.size == count);
        CHECK(payload.materials.max_size == count);
    }

    SUBCASE("Light")
    {
        constexpr auto count = 1;
        CHECK(payload.lights.size == count);
        CHECK(payload.lights.max_size == count);
    }

    SUBCASE("Primitive")
    {
        constexpr auto count = 8;
        CHECK(payload.primitives.size == count);
        CHECK(payload.primitives.max_size == count);
    }
}
