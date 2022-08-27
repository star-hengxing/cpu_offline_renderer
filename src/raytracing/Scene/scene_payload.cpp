#include <fstream>
#include <cassert>
// forward declare
#include "scene_payload.hpp"
#include "Scene.hpp"

#include <raytracing/Core/Primitive/geometry_primitive.hpp>

#include <raytracing/Core/Shape/Rectangle.hpp>

#include <raytracing/Core/Material/Matte.hpp>

#include <raytracing/Core/Light/Light.hpp>

#include <util/util.hpp>
#include <util/marco.hpp>

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
    auto matrix = Matrix4f::identity();
    if (!j["scale"].is_null())
    {
        const auto [x, y, z] = get_float3<Vector3, f32>(j, "scale");
        matrix               = Transform<f32>::scale(x, y, z) * matrix;
    }

    if (!j["rotate"].is_null())
    {
        const auto axis  = j["rotate"]["Axis"].get<Axis>();
        const auto angle = j["rotate"]["angle"].get<f32>();
        if (axis == Axis::X)
        {
            matrix = Transform<f32>::rotate<Axis::X>(angle) * matrix;
        }
        else if (axis == Axis::Y)
        {
            matrix = Transform<f32>::rotate<Axis::Y>(angle) * matrix;
        }
        else if (axis == Axis::Z)
        {
            matrix = Transform<f32>::rotate<Axis::Z>(angle) * matrix;
        }
        else
        {}
    }
    if (!j["translate"].is_null())
    {
        const auto v = get_float3<Vector3, f32>(j, "translate");

        matrix = Transform<f32>::translate(v) * matrix;
    }
    return matrix;
}

template <typename T>
auto release(T** ptr, usize count)
{
    for (auto i : range(count))
    {
        delete ptr[i];
    }
    return ptr + count;
};

NAMESPACE_END()

Matrix4f* scene_payload::add_matrix(const Matrix4f& matrix)
{
    assert(matrix_count < matrix_max_count);
    const auto ptr = (Matrix4f*)data.get() + matrix_count;

    ptr[0] = matrix;
    ptr[1] = matrix.inverse();
    matrix_count += 2;
    return ptr;
}

void scene_payload::add_shape(Shape* shape)
{
    assert(shape_count < shape_max_count);
    const auto mat = (Matrix4f*)data.get() + matrix_max_count;
    auto       ptr = (usize*)mat + shape_count;

    *((Shape**)ptr) = shape;
    shape_count += 1;
}

void scene_payload::add_material(Material* material)
{
    assert(material_count < material_max_count);
    const auto mat = (Matrix4f*)data.get() + matrix_max_count;
    auto       ptr = (usize*)mat + shape_max_count + material_count;

    *((Material**)ptr) = material;
    material_count += 1;
}

void scene_payload::add_light(Light* light)
{
    assert(light_count < light_max_count);
    const auto mat = (Matrix4f*)data.get() + matrix_max_count;
    auto       ptr = (usize*)mat + shape_max_count + material_max_count + light_count;

    *((Light**)ptr) = light;
    light_count += 1;
}

void scene_payload::add_primitive(geometry_primitive* primitive)
{
    assert(primitive_count < primitive_max_count);
    const auto mat = (Matrix4f*)data.get() + matrix_max_count;
    auto       ptr = (usize*)mat + shape_max_count + material_max_count + light_max_count + primitive_count;

    *((geometry_primitive**)ptr) = primitive;
    primitive_count += 1;
}

Matrix4f* scene_payload::get_transform(const json& j)
{
    if (j.is_null())
    {
        return (Matrix4f*)data.get();
    }

    const auto matrix = get_matrix(j);
    return add_matrix(matrix);
}

void scene_payload::counter_pass(const json& j)
{
    for (auto& j : j["Shapes"])
    {
        if (!j["transform"].is_null())
            matrix_max_count += 2;
    }

    for (auto& j : j["Lights"])
    {
        if (!j["Shape"]["transform"].is_null())
            light_max_count += 1;
    }

    shape_max_count     = j["Shapes"].size() + j["Lights"].size();
    material_max_count  = j["Materials"].size();
    primitive_max_count = shape_max_count;

    const auto size = matrix_max_count * sizeof(Matrix4f) + (shape_max_count + material_max_count + light_max_count + primitive_max_count) * sizeof(void*);

    data                     = std::make_unique<u8[]>(size);
    *((Matrix4f*)data.get()) = Matrix4f::identity();
}

void scene_payload::config_pass(const json& j)
{
    config.spp      = j["spp"].get<u32>();
    config.thread   = j["thread"].get<u32>();
    const auto name = j["thread"].get<std::string_view>();
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
        throw std::runtime_error("invalid integrator type");
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

}

void scene_payload::material_pass(const json& j)
{

}

void scene_payload::light_pass(const json& j)
{

}

bool scene_payload::load(const char* path)
{
    std::ifstream in("test.json");
    if (in.fail())
    {
        return false;
    }

    auto json = json::parse(in);
    counter_pass(json);
    config_pass(json["Config"]);
    camera_pass(json["Camera"]);
    shape_pass(json["Shapes"]);
    material_pass(json["Materials"]);
    light_pass(json["Lights"]);

    return true;
}

Scene scene_payload::get_scene() const
{
    Scene      scene;
    const auto mat = (Matrix4f*)data.get() + matrix_max_count;
    auto       ptr = (usize*)mat + shape_max_count + material_max_count;

    scene.lights = {(Light**)ptr, light_max_count};
    ptr += light_max_count;
    scene.primitives = {(geometry_primitive**)ptr, primitive_max_count};

    return scene;
}

void scene_payload::clean()
{
    auto matrix   = (Matrix4f*)data.get() + matrix_max_count;
    auto shape    = release((Shape**)matrix, shape_max_count);
    auto material = release((Material**)shape, material_max_count);
    auto light    = release((Light**)material, light_max_count);
    release((geometry_primitive**)light, primitive_max_count);
}

// void scene_payload::add_rectangle(const json& j)
// {
//     const auto transform = get_transform(j["transform"]);
//     const auto length    = j["length"].get<f32>();
//     const auto width     = j["width"].get<f32>();

//     auto shape     = new Rectangle(transform[0], transform[1], length, width);
//     auto material  = get_material(j["material"]);
//     auto primitive = new geometry_primitive(shape, material);

//     add_shape(shape);
//     add_primitive(primitive);
// }

// auto get_constant_texture(const json& j)
// {
//     auto r = j["value"][0].get<u8>();
//     auto g = j["value"][1].get<u8>();
//     auto b = j["value"][2].get<u8>();

//     const auto value = rgb_to_float<f32>(Color(r, g, b));
// }
