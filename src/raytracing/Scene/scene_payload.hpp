#pragma once

#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <math/basic_type.hpp>

#include <raytracing/Core/Camera/perspective_camera.hpp>

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
    u32 spp;
    u32 thread;
    integrator_type type;
};

struct scene_payload
{
public:
    u32 matrix_count    = 1;
    u32 shape_count     = 0;
    u32 material_count  = 0;
    u32 light_count     = 0;
    u32 primitive_count = 0;

    u32 matrix_max_count    = 1;
    u32 shape_max_count     = 0;
    u32 material_max_count  = 0;
    u32 light_max_count     = 0;
    u32 primitive_max_count = 0;

    std::unique_ptr<u8[]> data;

    Config             config;
    perspective_camera camera;

private:
    Matrix4f* add_matrix(const Matrix4f& matrix);
    void      add_shape(Shape* shape);
    void      add_material(Material* material);
    void      add_light(Light* light);
    void      add_primitive(geometry_primitive* primitive);

    Matrix4f* get_transform(const json& j);
    Shape*    get_shape(const json& j);
    Material* get_material(const json& j);
    Light*    get_light(const json& j);

    void add_rectangle(const json& j);

    void counter_pass(const json& j);
    void config_pass(const json& j);
    void camera_pass(const json& j);
    void shape_pass(const json& j);
    void material_pass(const json& j);
    void light_pass(const json& j);

public:
    bool load(const char* path);

    Scene get_scene() const;

    void clean();
};
