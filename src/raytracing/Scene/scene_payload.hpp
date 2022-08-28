#pragma once

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <raytracing/Core/Camera/perspective_camera.hpp>
#include <math/basic_type.hpp>

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
    u32 matrix_count    = 0;
    u32 shape_count     = 0;
    u32 material_count  = 0;
    u32 light_count     = 0;
    u32 primitive_count = 0;

    u32 matrix_max_count    = 0;
    u32 shape_max_count     = 0;
    u32 material_max_count  = 0;
    u32 light_max_count     = 0;
    u32 primitive_max_count = 0;

    Matrix4f*            matrixes;
    Shape**              shapes;
    Material**           materials;
    Light**              lights;
    geometry_primitive** primitives;

    std::unique_ptr<u8[]>    data;
    std::vector<const char*> materials_name;

    Config             config;
    perspective_camera camera;

private:
    Matrix4f*            get_matrix_position();
    Shape**              get_shape_position();
    Material**           get_material_position();
    Light**              get_light_position();
    geometry_primitive** get_primitive_position();

    Matrix4f* add_matrix(const Matrix4f& matrix);
    void      add_shape(Shape* shape);
    void      add_material(Material* material);
    void      add_light(Light* light);
    void      add_primitive(geometry_primitive* primitive);

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

    Material* make_matte(const json& j);

    Light* make_area_light(const json& j);

public:
    bool load(const char* path);

    Scene get_scene() const;

    void clean();
};
