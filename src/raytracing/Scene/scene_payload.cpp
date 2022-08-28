#include <optional>
#include <fstream>
#include <cassert>
#include <memory>
// forward declare
#include "scene_payload.hpp"
#include "Scene.hpp"

#include <raytracing/Core/Primitive/geometry_primitive.hpp>

#include <raytracing/Core/Shape/Rectangle.hpp>

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

template <typename T>
auto release(T** ptr, usize count)
{
    for (auto i : range(count))
    {
        delete ptr[i];
    }
    return ptr + count;
};

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

Matrix4f* scene_payload::get_matrix_position()
{
    assert(matrix_count <= matrix_max_count);
    return matrixes + matrix_count;
}

Shape** scene_payload::get_shape_position()
{
    assert(shape_count <= shape_max_count);
    return shapes + shape_count;
}

Material** scene_payload::get_material_position()
{
    assert(material_count <= material_max_count);
    return materials + material_count;
}

Light** scene_payload::get_light_position()
{
    assert(light_count <= light_max_count);
    return lights + light_count;
}

geometry_primitive** scene_payload::get_primitive_position()
{
    assert(primitive_count <= primitive_max_count);
    return primitives + primitive_count;
}

Matrix4f* scene_payload::add_matrix(const Matrix4f& matrix)
{
    const auto ptr = get_matrix_position();

    ptr[0] = matrix;
    ptr[1] = matrix.inverse();
    matrix_count += 2;
    return ptr;
}

void scene_payload::add_shape(Shape* shape)
{
    const auto ptr = get_shape_position();

    *ptr = shape;
    shape_count += 1;
}

void scene_payload::add_material(Material* material)
{
    const auto ptr = get_material_position();

    *ptr = material;
    material_count += 1;
}

void scene_payload::add_light(Light* light)
{
    const auto ptr = get_light_position();

    *ptr = light;
    light_count += 1;
}

void scene_payload::add_primitive(geometry_primitive* primitive)
{
    const auto ptr = get_primitive_position();

    *ptr = primitive;
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
    matrix_max_count += 1;
    for (auto& j : j["Shapes"])
    {
        if (!j["transform"].is_null())
            matrix_max_count += 2;
    }

    for (auto& j : j["Lights"])
    {
        if (!j["shape"]["transform"].is_null())
            matrix_max_count += 2;
    }

    light_max_count     = j["Lights"].size();
    shape_max_count     = j["Shapes"].size() + light_max_count;
    material_max_count  = j["Materials"].size();
    primitive_max_count = shape_max_count;
}

void scene_payload::init_pass()
{
    const auto size = matrix_max_count * sizeof(Matrix4f) + (shape_max_count + material_max_count + light_max_count + primitive_max_count) * sizeof(void*);

    data = std::make_unique<u8[]>(size);

    matrixes    = (Matrix4f*)data.get();
    matrixes[0] = Matrix4f::identity();
    matrix_count += 1;

    shapes     = (Shape**)(matrixes + matrix_max_count);
    materials  = (Material**)(shapes + shape_max_count);
    lights     = (Light**)(materials + material_max_count);
    primitives = (geometry_primitive**)(lights + light_max_count);
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
        add_shape(shape);
        add_primitive(primitive);
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
        add_material(material);
        materials_name.emplace_back(name.data());
    }
}

void scene_payload::light_pass(const json& j)
{
    for (auto i : range(j.size()))
    {
        const auto& value = j[i];
        const auto  shape = make_shape(value["shape"]);
        add_shape(shape);

        const auto light     = make_light(value);
        const auto primitive = new geometry_primitive(shape, nullptr, light);
        add_light(light);
        add_primitive(primitive);
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
    scene.lights     = {lights, light_max_count};
    scene.primitives = {primitives, primitive_max_count};
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

Shape* scene_payload::make_shape(const json& j)
{
    Shape*     shape;
    const auto type = j["type"].get<std::string_view>();

    if (type == "Rectangle")
    {
        shape = make_rectangle(j);
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
    return new Rectangle(transform[0], transform[1], length, width);
}

Light* scene_payload::make_area_light(const json& j)
{
    const auto emit  = Spectrum{j["emit"].get<f32>()};
    const auto shape = *(get_shape_position() - 1);
    return new area_light(shape, emit);
}

Material* scene_payload::make_matte(const json& j)
{
    auto albedo = make_texture(j["albedo"]);
    return new Matte(std::move(albedo));
}
