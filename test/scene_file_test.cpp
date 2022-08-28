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
        CHECK(payload.matrix_count == count);
        CHECK(payload.matrix_max_count == count);
    }

    SUBCASE("Shape")
    {
        constexpr auto count = 8;
        CHECK(payload.shape_count == count);
        CHECK(payload.shape_max_count == count);
    }

    SUBCASE("Material")
    {
        constexpr auto count = 3;
        CHECK(payload.material_count == count);
        CHECK(payload.material_max_count == count);
    }

    SUBCASE("Light")
    {
        constexpr auto count = 1;
        CHECK(payload.light_count == count);
        CHECK(payload.light_max_count == count);
    }

    SUBCASE("Primitive")
    {
        constexpr auto count = 8;
        CHECK(payload.primitive_count == count);
        CHECK(payload.primitive_max_count == count);
    }
}
