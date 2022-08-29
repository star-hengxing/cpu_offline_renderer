#include <optional>
#include <fstream>
#include <cassert>
#include <memory>
// forward declare
#include "scene_payload.hpp"
#include "Scene.hpp"

#include <raytracing/Core/Primitive/geometry_primitive.hpp>
#include <raytracing/Core/Material/Material.hpp>
#include <raytracing/Core/Shape/Shape.hpp>
#include <raytracing/Core/Light/Light.hpp>

#include <util/io.hpp>

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
