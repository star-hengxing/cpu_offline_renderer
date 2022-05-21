// 屎山，等以后学有所成，用优雅的方式重构
#include <make_scene.hpp>

#include <util/Color/color_space.hpp>

#include <Core/Integrator/path_tracing_integrator.hpp>

#include <Core/Primitive/geometry_primitive.hpp>

#include <Core/Texture/constant_texture.hpp>
#include <Core/Texture/checker_texture.hpp>

#include <Core/Light/area_light.hpp>

#include <Core/Material/Matte.hpp>

#include <Core/Shape/Rectangle.hpp>
#include <Core/Shape/Cuboid.hpp>

#include <util/util.hpp>

#include <Hinae/Transform.hpp>

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
        Point3f{0, 5, 20}, Point3f{0, 5, 0}, Vector3f{0, 1, 0},
        45
    };

    Scene scene;

    const auto matte_white = make_matte<constant_texture<Spectrum>>(RGB::white);
    const auto matte_red = make_matte<constant_texture<Spectrum>>(RGB::red);
    const auto matte_green = make_matte<constant_texture<Spectrum>>(RGB::green * 0.5);

    const auto matte_checker = make_matte<checker_texture<Spectrum>>(4, 4, RGB::white, RGB::cyan);

    {
        constexpr f32 length = 5;
        const auto t_back = translate_rotate<Axis::X>({0, length, -length}, 90);
        const auto t_left = translate_rotate<Axis::Z>({-length, length, 0}, 270);
        const auto t_right = translate_rotate<Axis::Z>({length, length, 0}, 90);
        const auto t_ceil = Transform<f32>::translate({0, length * 2, 0});

        make_rectangle(scene, Matrix4f::identity(), length, length, matte_checker);
        make_rectangle(scene, t_back, length, length, matte_white);
        make_rectangle(scene, t_left, length, length, matte_red);
        make_rectangle(scene, t_right, length, length, matte_green);
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
    unimplemented();
}
