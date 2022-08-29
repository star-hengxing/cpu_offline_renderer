#include <optional>
#include <fstream>
#include <cassert>
#include <memory>
// forward declare
#include "scene_payload.hpp"
#include "Scene.hpp"

#include <raytracing/Core/Primitive/geometry_primitive.hpp>

#include <raytracing/Core/Shape/Rectangle.hpp>
#include <raytracing/Core/Shape/Cuboid.hpp>

#include <raytracing/Core/Material/Matte.hpp>

#include <raytracing/Core/Texture/constant_texture.hpp>

#include <raytracing/Core/Light/area_light.hpp>

#include <util/marco.hpp>
#include <util/util.hpp>
#include <util/io.hpp>

#include <math/Transform.hpp>

NAMESPACE_BEGIN()

template <template <arithmetic> typename T, arithmetic U>
constexpr T<U> get_float3(const json& j, const char* filed)
{
    T<U> ret;
    for (auto i : range(3))
    {
        ret[i] = j[filed][i].get<U>();
    }
    return ret;
}

Matrix4f get_matrix(const json& j)
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

Matrix4f* scene_payload::get_transform(const json& j)
{
    if (j.is_null())
    {
        return matrixes.ptr;
    }

    const auto matrix  = get_matrix(j);
    const auto current = matrixes.next_ptr();
    matrixes.add(matrix)
        .add(matrix.inverse());
    return current;
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

void scene_payload::counter_pass(const json& j)
{
    u32 max_matrix_size = 1;
    for (auto& j : j["Shapes"])
    {
        if (!j["transform"].is_null())
            max_matrix_size += 2;
    }

    for (auto& j : j["Lights"])
    {
        if (!j["shape"]["transform"].is_null())
            max_matrix_size += 2;
    }

    matrixes.max_size   = max_matrix_size;
    lights.max_size     = j["Lights"].size();
    shapes.max_size     = j["Shapes"].size() + lights.max_size;
    materials.max_size  = j["Materials"].size();
    primitives.max_size = shapes.max_size;
}

void scene_payload::init_pass()
{
    const auto size = matrixes.max_size * sizeof(Matrix4f) + (shapes.max_size + materials.max_size + lights.max_size + primitives.max_size) * sizeof(void*);

    data = std::make_unique<u8[]>(size);

    matrixes.ptr = (Matrix4f*)data.get();
    matrixes.add(Matrix4f::identity());

    shapes.ptr     = (Shape**)(matrixes.ptr + matrixes.max_size);
    materials.ptr  = (Material**)(shapes.ptr + shapes.max_size);
    lights.ptr     = (Light**)(materials.ptr + materials.max_size);
    primitives.ptr = (geometry_primitive**)(lights.ptr + lights.max_size);
}

void scene_payload::config_pass(const json& j)
{
    config.spp      = j["spp"].get<u32>();
    config.thread   = j["thread"].get<u32>();
    const auto name = j["Integrator"].get<std::string_view>();
    if (name == "ao")
    {
        config.type = integrator_type::ao;
    }
    else if (name == "direct")
    {
        config.type = integrator_type::direct;
    }
    else if (name == "pt")
    {
        config.type = integrator_type::pt;
    }
    else
    {
        println("invalid integrator type");
        exit(-1);
    }
}

void scene_payload::camera_pass(const json& j)
{
    const auto width     = j["width"].get<usize>();
    const auto height    = j["height"].get<usize>();
    const auto pos       = get_float3<Point3, f32>(j, "pos");
    const auto at        = get_float3<Point3, f32>(j, "at");
    const auto up        = get_float3<Vector3, f32>(j, "up");
    const auto fov_angle = j["fov_angle"].get<f32>();

    camera = perspective_camera{
        width, height,
        pos, at, up,
        fov_angle};
}

void scene_payload::shape_pass(const json& j)
{
    for (auto i : range(j.size()))
    {
        const auto& value     = j[i];
        const auto  shape     = make_shape(value);
        const auto  material  = get_material(value["material"]);
        const auto  primitive = new geometry_primitive(shape, material);
        shapes.add(shape);
        primitives.add(primitive);
    }
}

void scene_payload::material_pass(const json& j)
{
    materials_name.reserve(j.size());
    for (auto i : range(j.size()))
    {
        const auto& value    = j[i];
        const auto  name     = value["name"].get<std::string_view>();
        const auto  material = make_material(value);
        materials.add(material);
        materials_name.emplace_back(name.data());
    }
}

void scene_payload::light_pass(const json& j)
{
    for (auto i : range(j.size()))
    {
        const auto& value = j[i];
        const auto  shape = make_shape(value["shape"]);
        shapes.add(shape);

        const auto light     = make_light(value);
        const auto primitive = new geometry_primitive(shape, nullptr, light);
        lights.add(light);
        primitives.add(primitive);
    }
}

bool scene_payload::load(const char* path)
{
    std::ifstream in(path);
    if (in.fail())
    {
        println("Failed to open scene file");
        return false;
    }

    const auto json = json::parse(in);
    counter_pass(json);
    init_pass();
    config_pass(json["Config"]);
    camera_pass(json["Camera"]);
    // init material first
    material_pass(json["Materials"]);
    shape_pass(json["Shapes"]);
    light_pass(json["Lights"]);

    materials_name.clear();
    return true;
}

Scene scene_payload::get_scene() const
{
    Scene scene;
    scene.lights     = lights.view();
    scene.primitives = primitives.view();
    return scene;
}

void scene_payload::clean()
{
    shapes.clean();
    materials.clean();
    lights.clean();
    primitives.clean();
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
    const auto transform = get_transform(j["transform"]);
    const auto length    = j["length"].get<f32>();
    const auto width     = j["width"].get<f32>();
    if (transform == matrixes.ptr)
    {
        return new Rectangle(transform[0], transform[0], length, width);
    }
    else
    {
        return new Rectangle(transform[0], transform[1], length, width);
    }
}

Shape* scene_payload::make_cuboid(const json& j)
{
    const auto transform = get_transform(j["transform"]);
    const auto length    = j["length"].get<f32>();
    const auto width     = j["width"].get<f32>();
    const auto height    = j["height"].get<f32>();
    if (transform == matrixes.ptr)
    {
        return new Cuboid(transform[0], transform[0], length, width, height);
    }
    else
    {
        return new Cuboid(transform[0], transform[1], length, width, height);
    }
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
