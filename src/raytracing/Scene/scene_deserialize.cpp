#include <cassert>
// forward declare
#include "scene_payload.hpp"

#include <raytracing/Core/Shape/Rectangle.hpp>
#include <raytracing/Core/Shape/Cuboid.hpp>

#include <raytracing/Core/Material/Matte.hpp>

#include <raytracing/Core/Texture/constant_texture.hpp>

#include <raytracing/Core/Light/area_light.hpp>

#include <math/Transform.hpp>

#include <util/marco.hpp>
#include <util/io.hpp>

NAMESPACE_BEGIN()

Matrix4f get_transform(const json& j)
{
    auto t = Matrix4f::identity();
    auto r = Matrix4f::identity();
    auto s = Matrix4f::identity();

    if (!j["translate"].is_null())
    {
        const auto v = get_float3<Vector3, f32>(j, "translate");

        t = Transform<f32>::translate(v);
    }

    if (!j["rotate"].is_null())
    {
        const auto axis  = j["rotate"]["Axis"].get<Axis>();
        const auto angle = j["rotate"]["angle"].get<f32>();
        if (axis == Axis::X)
        {
            r = Transform<f32>::rotate<Axis::X>(angle);
        }
        else if (axis == Axis::Y)
        {
            r = Transform<f32>::rotate<Axis::Y>(angle);
        }
        else if (axis == Axis::Z)
        {
            r = Transform<f32>::rotate<Axis::Z>(angle);
        }
        else
        {
            println("invalid axis type");
            exit(-1);
        }
    }

    if (!j["scale"].is_null())
    {
        const auto [x, y, z] = get_float3<Vector3, f32>(j, "scale");

        s = Transform<f32>::scale(x, y, z);
    }

    return t * r * s;
}

std::unique_ptr<Texture> make_constant_texture(const json& j)
{
    const auto r      = j["albedo"][0].get<u8>();
    const auto g      = j["albedo"][1].get<u8>();
    const auto b      = j["albedo"][2].get<u8>();
    const auto albedo = rgb_to_float<f32>(Color(r, g, b));
    return std::make_unique<constant_texture>(albedo);
}

std::unique_ptr<Texture> make_texture(const json& j)
{
    std::unique_ptr<Texture> texture;

    const auto type = j["type"].get<std::string_view>();

    if (type == "constant_texture")
    {
        texture = make_constant_texture(j);
    }
    else
    {
        println("invalid texture type");
        exit(-1);
    }
    return texture;
}

NAMESPACE_END()

std::tuple<const Matrix4f&, const Matrix4f&>
scene_payload::make_transform(const json& j)
{
    if (j.is_null())
    {
        return {matrixes[0], matrixes[0]};
    }

    const auto matrix  = get_transform(j);
    const auto current = matrixes.next_ptr();
    matrixes.add(matrix)
        .add(matrix.inverse());

    return {current[0], current[1]};
}

Material* scene_payload::get_material(const json& j)
{
    const auto name = j.get<std::string_view>();

    std::optional<usize> index;
    for (const auto& [i, value] : enumerate(materials_name))
    {
        if (name == value)
            index = i;
    }

    if (!index.has_value())
    {
        println("invalid material name");
        exit(-1);
    }

    return materials[index.value()];
}

Shape* scene_payload::make_shape(const json& j)
{
    Shape*     shape;
    const auto type = j["type"].get<std::string_view>();

    if (type == "Rectangle")
    {
        shape = make_rectangle(j);
    }
    else if (type == "Cuboid")
    {
        shape = make_cuboid(j);
    }
    else
    {
        println("invalid shape type");
        exit(-1);
    }
    return shape;
}

Material* scene_payload::make_material(const json& j)
{
    Material*  material;
    const auto type = j["type"].get<std::string_view>();

    if (type == "Matte")
    {
        material = make_matte(j);
    }
    else
    {
        println("invalid shape type");
        exit(-1);
    }
    return material;
}

Light* scene_payload::make_light(const json& j)
{
    Light*     light;
    const auto type = j["type"].get<std::string_view>();

    if (type == "area_light")
    {
        light = make_area_light(j);
    }
    else
    {
        println("invalid light type");
        exit(-1);
    }
    return light;
}

Shape* scene_payload::make_rectangle(const json& j)
{
    const auto [m, inv] = make_transform(j["transform"]);
    const auto length   = j["length"].get<f32>();
    const auto width    = j["width"].get<f32>();
    return new Rectangle(m, inv, length, width);
}

Shape* scene_payload::make_cuboid(const json& j)
{
    const auto [m, inv] = make_transform(j["transform"]);
    const auto length   = j["length"].get<f32>();
    const auto width    = j["width"].get<f32>();
    const auto height   = j["height"].get<f32>();
    return new Cuboid(m, inv, length, width, height);
}

Light* scene_payload::make_area_light(const json& j)
{
    const auto emit  = Spectrum{j["emit"].get<f32>()};
    const auto shape = shapes.back();
    return new area_light(shape, emit);
}

Material* scene_payload::make_matte(const json& j)
{
    auto albedo = make_texture(j["albedo"]);
    return new Matte(std::move(albedo));
}
