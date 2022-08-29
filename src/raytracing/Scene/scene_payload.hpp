#pragma once

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <raytracing/Core/Camera/perspective_camera.hpp>
#include <util/basic_type.hpp>
#include <util/Helper.hpp>

struct Scene;

struct Shape;
struct Material;
struct Light;
struct geometry_primitive;

enum class integrator_type
{
    ao,
    direct,
    pt,
};

struct Config
{
    u32             spp;
    u32             thread;
    integrator_type type;
};

struct scene_payload
{
public:
    Helper<Matrix4f>            matrixes;
    Helper<Shape*>              shapes;
    Helper<Material*>           materials;
    Helper<Light*>              lights;
    Helper<geometry_primitive*> primitives;

    std::unique_ptr<u8[]>    data;
    std::vector<const char*> materials_name;

    Config             config;
    perspective_camera camera;

private:
    Matrix4f* get_transform(const json& j);
    Shape*    get_shape(const json& j);
    Material* get_material(const json& j);
    Light*    get_light(const json& j);

    void counter_pass(const json& j);
    void init_pass();
    void config_pass(const json& j);
    void camera_pass(const json& j);
    void shape_pass(const json& j);
    void material_pass(const json& j);
    void light_pass(const json& j);

    Shape*    make_shape(const json& j);
    Material* make_material(const json& j);
    Light*    make_light(const json& j);

    Shape* make_rectangle(const json& j);
    Shape* make_cuboid(const json& j);

    Material* make_matte(const json& j);

    Light* make_area_light(const json& j);

public:
    bool load(const char* path);

    Scene get_scene() const;

    void clean();
};
