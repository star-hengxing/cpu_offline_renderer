// 屎山，等以后学有所成，用优雅的方式重构
#include <raytracing/make_scene.hpp>

#include <util/Color/color_space.hpp>

#include <raytracing/Core/Integrator/path_tracing_integrator.hpp>
#include <raytracing/Core/Integrator/direct_light_integrator.hpp>

#include <raytracing/Core/Primitive/geometry_primitive.hpp>

#include <raytracing/Core/Texture/constant_texture.hpp>
#include <raytracing/Core/Texture/checker_texture.hpp>

#include <raytracing/Core/Light/area_light.hpp>

#include <raytracing/Core/Material/Matte.hpp>
#include <raytracing/Core/Material/Glossy.hpp>
#include <raytracing/Core/Material/Glass.hpp>
#include <raytracing/Core/Material/Metal.hpp>

#include <raytracing/Core/Shape/Rectangle.hpp>
#include <raytracing/Core/Shape/Cuboid.hpp>
#include <raytracing/Core/Shape/Sphere.hpp>
#include <raytracing/Core/Shape/Annulus.hpp>
#include <raytracing/Core/Shape/Torus.hpp>

#include <util/util.hpp>

#include <math/Transform.hpp>

void make_rectangle(Scene& scene, const Matrix4f& transform, f32 length, f32 width, std::shared_ptr<Material> material)
{
    if(transform == Matrix4f::identity())
    {
        const auto& m = scene.identity();
        scene.add(std::make_shared<geometry_primitive>(std::make_shared<Rectangle>(m, m, length, width), material));
    }
    else
    {
        const auto& [m, inv] = scene.add(transform);
        scene.add(std::make_shared<geometry_primitive>(std::make_shared<Rectangle>(m, inv, length, width), material));
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

return_type cornell_box()
{
    auto camera = perspective_camera
    {
        700, 700,
        Point3f{0, 5, 17}, Point3f{0, 5, 0}, Vector3f{0, 1, 0},
        45
    };

    Scene scene;

    const Spectrum light_purple = rgb_to_float<f32>(Color(147,112,219));
    const Spectrum light_pink = rgb_to_float<f32>(Color(255, 182, 193));

    const auto matte_white = make_matte<constant_texture<Spectrum>>(RGB::white);
    const auto matte_light_pink = make_matte<constant_texture<Spectrum>>(light_pink);
    const auto matte_light_purple = make_matte<constant_texture<Spectrum>>(light_purple);

    const auto matte_checker = make_matte<checker_texture<Spectrum>>(16, 16, RGB::white, RGB::cyan);

    {
        constexpr f32 length = 5;
        const auto t_back = translate_rotate<Axis::X>({0, length, -length}, 90);
        const auto t_left = translate_rotate<Axis::Z>({-length, length, 0}, 270);
        const auto t_right = translate_rotate<Axis::Z>({length, length, 0}, 90);
        const auto t_ceil = Transform<f32>::translate({0, length * 2, 0});

        make_rectangle(scene, Matrix4f::identity(), length, length, matte_white);
        make_rectangle(scene, t_back, length, length, matte_white);
        make_rectangle(scene, t_left, length, length, matte_light_pink);
        make_rectangle(scene, t_right, length, length, matte_light_purple);
        make_rectangle(scene, t_ceil, length, length, matte_white);
    }
    // left Cuboid
    {
        const auto t = translate_rotate<Axis::Y>({-2, 3, -2}, 20);
        const auto& [m, inv] = scene.add(t);
        const auto shape = std::make_shared<Cuboid>(m, inv, 1.5, 1.5, 3);
        scene.add(std::make_shared<geometry_primitive>(shape, matte_white));
    }
    // right Cuboid
    {
        const auto t = translate_rotate<Axis::Y>({1.5, 1.5, 1.5}, -15);
        const auto& [m, inv] = scene.add(t);
        const auto shape = std::make_shared<Cuboid>(m, inv, 1.5, 1.5, 1.5);
        scene.add(std::make_shared<geometry_primitive>(shape, matte_white));
    }
    // left Sphere
    {
        const auto t = Transform<f32>::translate({1.5, 4, 1.5});
        const auto& [m, inv] = scene.add(t);
        const auto shape = std::make_shared<Sphere>(m, inv, 1);
        constexpr std::array<f32, 3> eta{0.15494f, 0.11648f, 0.13809f};
        constexpr std::array<f32, 3> k{4.81810f, 3.11562f, 2.14240f};
        const auto material = std::make_shared<Metal>(0.001, 0.001, eta, k);
        scene.add(std::make_shared<geometry_primitive>(shape, material));
    }
    // right Sphere
    {
        const auto t = Transform<f32>::translate({-2, 1, 2});
        const auto& [m, inv] = scene.add(t);
        const auto shape = std::make_shared<Sphere>(m, inv, 1);
        const auto material = std::make_shared<Glossy>(light_purple, 0.1, 0.1, 0.5);
        scene.add(std::make_shared<geometry_primitive>(shape, material));
    }

    {
        const auto rectangle_light_transform = translate_rotate<Axis::X>({0, 9.99, 0}, 180);
        const auto& [m, inv] = scene.add(rectangle_light_transform);
        const auto shape = std::make_shared<Rectangle>(m, inv, 1, 1);
        const auto light = std::make_shared<area_light>(shape, Spectrum{50});
        scene.add(light);
        scene.add(std::make_shared<geometry_primitive>(shape, nullptr, light));
    }

    return std::make_tuple
    (
        std::move(scene), std::move(camera),
        std::make_unique<path_tracing_integrator>()
    );
}

return_type native()
{
    auto camera = perspective_camera
    {
        1280, 720,
        Point3f{0, 5, 20}, Point3f{0, 5, 0}, Vector3f{0, 1, 0},
        45
    };

    Scene scene;

    const Spectrum light_purple = rgb_to_float<f32>(Color(147,112,219));
    const Spectrum light_pink = rgb_to_float<f32>(Color(255, 182, 193));

    const auto matte_white = make_matte<constant_texture<Spectrum>>(RGB::white);
    const auto matte_light_pink = make_matte<constant_texture<Spectrum>>(light_pink);
    const auto matte_light_purple = make_matte<constant_texture<Spectrum>>(light_purple);

    constexpr auto size = 64;
    const auto matte_checker = make_matte<checker_texture<Spectrum>>(size, size, RGB::white, light_pink);

    {
        constexpr auto length = 128;
        make_rectangle(scene, Matrix4f::identity(), length, length, matte_checker);
    }

    // {
    //     constexpr std::string_view path = "a:/resource/model/mary/Marry.obj";
    //     const auto t = Transform<f32>::translate({0, 0, -2}) * Transform<f32>::scale(2);
    //     scene.add(path, t, matte_light_pink);
    // }

    // {
    //     const auto t = Transform<f32>::translate({-5, 2, 5});
    //     const auto& [m, inv] = scene.add(t);
    //     const auto shape = std::make_shared<Sphere>(m, inv, 1);
    //     const auto material = std::make_shared<Glass>(0.01, 0.01, 1.5);
    //     scene.add(std::make_shared<geometry_primitive>(shape, material));
    // }

    {
        const auto t = Transform<f32>::translate({0, 2, 0});
        const auto& [m, inv] = scene.add(t);
        const auto shape = std::make_shared<Torus>(m, inv, 2, 1);
        const auto material = make_matte<constant_texture<Spectrum>>(RGB::dodger_blue);
        scene.add(std::make_shared<geometry_primitive>(shape, material));
    }

    {
        const auto rectangle_light_transform = translate_rotate<Axis::X>({0, 10, 0}, 180);
        const auto& [m, inv] = scene.add(rectangle_light_transform);
        const auto shape = std::make_shared<Annulus>(m, inv, 1, 0, 360);
        const auto light = std::make_shared<area_light>(shape, Spectrum{50});
        scene.add(light);
        scene.add(std::make_shared<geometry_primitive>(shape, nullptr, light));
    }

    return std::make_tuple
    (
        std::move(scene), std::move(camera),
        std::make_unique<path_tracing_integrator>()
    );
}
