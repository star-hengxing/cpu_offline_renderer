#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <raytracing/Scene/scene_payload.hpp>

TEST_CASE("testing the scene file format")
{
    scene_payload payload;
    REQUIRE(payload.load("asset/cornell_box.json"));

    SUBCASE("Shape")
    {
        constexpr auto count = 2;
        CHECK(payload.shape_count == count);
        CHECK(payload.shape_max_count == count);
    }

    SUBCASE("Material")
    {
        constexpr auto count = 0;
        CHECK(payload.material_count == count);
        CHECK(payload.material_max_count == count);
    }

    SUBCASE("Light")
    {
        constexpr auto count = 1;
        CHECK(payload.light_count == count);
        CHECK(payload.light_max_count == count);
    }
}
