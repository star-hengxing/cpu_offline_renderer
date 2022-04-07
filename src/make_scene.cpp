// 屎山，等以后学有所成，用优雅的方式重构
#include <make_scene.hpp>

#include <util/Parser.hpp>

#include <util/Color/color_space.hpp>
#include <util/Color/Color.hpp>
#include <util/util.hpp>

#include <Core/Integrator/path_tracing_integrator.hpp>
#include <Core/Integrator/direct_light_integrator.hpp>

#include <Core/Primitive/geometry_primitive.hpp>

#include <Core/Material/Mirror.hpp>
#include <Core/Material/Matte.hpp>

#include <Core/Texture/constant_texture.hpp>
#include <Core/Texture/checker_texture.hpp>

#include <Core/Light/point_light.hpp>

#include <Core/Shape/Sphere.hpp>
#include <Core/Shape/Plane.hpp>
#include <Core/Shape/Cube.hpp>

#include <Hinae/Transform.hpp>

void make_sphere(Scene& scene, const Vector3f& translate, f32 radius
    , std::shared_ptr<Material> material)
{
    if(translate == Vector3f{0})
    {
        const auto& m = scene.identity();
        scene.add(std::make_shared<geometry_primitive>(std::make_shared<Sphere>(m, m, radius), material));
    }
    else
    {
        const auto& [m, inv] = scene.add
        (
            Transform<f32>::translate(translate),
            Transform<f32>::translate(-translate)
        );
        scene.add(std::make_shared<geometry_primitive>(std::make_shared<Sphere>(m, inv, radius), material));
    }
}

void make_plane(Scene& scene, const Matrix4f& transform, f32 length
    , std::shared_ptr<Material> material)
{
    if(transform == Matrix4f::identity())
    {
        const auto& m = scene.identity();
        scene.add(std::make_shared<geometry_primitive>(std::make_shared<Plane>(m, m, length), material));
    }
    else
    {
        const auto& [m, inv] = scene.add(transform);
        scene.add(std::make_shared<geometry_primitive>(std::make_shared<Plane>(m, inv, length), material));
    }
}

template <Axis axis>
Matrix4f translate_rotate_scale(const Vector3f& t, f32 s, f32 angle)
{
    return Transform<f32>::translate(t) * Transform<f32>::rotate<axis>(angle) * Transform<f32>::scale(s);
}

template <Axis axis>
Matrix4f translate_rotate(const Vector3f& t, f32 angle)
{
    return Transform<f32>::translate(t) * Transform<f32>::rotate<axis>(angle);
}

template <typename T, typename... Args>
std::shared_ptr<Material> make_matte(Args... args)
{
    return std::make_shared<Matte>(std::make_shared<T>(std::forward<Args>(args)...));
}

std::tuple
<
    Scene,
    perspective_camera,
    std::unique_ptr<Integrator>
>
parse()
{
    auto camera = perspective_camera
    {
        1280, 720,
        Point3f{0, 5, 10}, Point3f{0, 0, 0}, Vector3f{0, 1, 0},
        60
    };

    Scene scene;

    constexpr f32 length = 10;

    auto m_back = translate_rotate<Axis::X>({0, length, -length}, 90);
    auto m_left = translate_rotate<Axis::Z>({-length, length, 0}, 270);
    auto m_right = translate_rotate<Axis::Z>({length, length, 0}, 90);
    // auto m_ceil = Transform<f32>::translate({0, length, 0});

    make_plane(scene, Matrix4f::identity(), length, make_matte<checker_texture<Spectrum>>(4, 4, RGB::white, RGB::cyan));
    make_plane(scene, m_back, length, make_matte<constant_texture<Spectrum>>(RGB::white));
    make_plane(scene, m_left, length, make_matte<constant_texture<Spectrum>>(RGB::red));
    make_plane(scene, m_right, length, make_matte<constant_texture<Spectrum>>(RGB::green));
    // make_plane(scene, m_ceil, length, make_matte<constant_texture<Spectrum>>(white));

    auto m_cube = Transform<f32>::translate({0, 1, 3});
    const auto& [m1, m2] = scene.add(m_cube);
    auto cube = std::make_shared<Cube>(m1, m2);
    scene.add(std::make_shared<geometry_primitive>(cube, make_matte<constant_texture<Spectrum>>(RGB::dodger_blue)));

    make_sphere(scene, {0, 4, -3}, 1, std::make_shared<Mirror>());
    make_sphere(scene, {3, 1, -3}, 1, make_matte<constant_texture<Spectrum>>(RGB::yellow));
    make_sphere(scene, {-3, 1, -3}, 1, make_matte<checker_texture<Spectrum>>(20, 20, RGB::white, RGB::black));

    scene.add(std::make_shared<point_light>(Point3f{0, 10, 0}, Spectrum{100}));

    return std::make_tuple
    (
        std::move(scene), std::move(camera),
        std::make_unique<path_tracing_integrator>()
    );
}
