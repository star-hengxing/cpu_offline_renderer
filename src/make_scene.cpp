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

template <typename T, usize size>
auto get(const std::vector<Object>& property)
{
    if constexpr(size == 1)
    {
        if constexpr(std::is_same_v<T, f32>)
            return std::atof(property.front().name.data());
        else
            return std::atoi(property.front().name.data());
    }
    else
    {
        T ret;
        for(auto i : range(size))
        {
            ret[i] = std::atof(property[i].name.data());
        }
        return ret;
    }
}

Matrix4f get_transform(const std::vector<Object>& objects)
{
    auto get_value = [](const std::vector<Object>& property, usize i) -> f32
    {
        return std::atof(property[i].name.data());
    };
    auto ret = Matrix4f::identity();
    for(const auto& property : objects)
    {
        if(property.name == "translate")
        {
            Vector3f v;
            for(auto i : range(3))
            {
                v[i] = get_value(property.property, i);
            }
            ret = Transform<f32>::translate(v) * ret;
        }
        else if(property.name == "rotate")
        {
            // Axis axis;
            // f32 angle;
            
        }
        else if(property.name == "scale")
        {
            f32 v[3];
            for(auto i : range(3))
            {
                v[i] = get_value(property.property, i);
            }
            ret = Transform<f32>::scale(v[0], v[1], v[2]) * ret;
        }
    }
    return ret;
}

Spectrum get_color(const std::string_view& sv)
{
    Spectrum color;
    if(sv == "white")
        color = RGB::white;
    else if(sv == "cyan")
        color = RGB::cyan;

    return color;
}

std::shared_ptr<Texture<Spectrum>> get_texture(const std::vector<Object>& objects)
{
    std::shared_ptr<Texture<Spectrum>> texture;
    const Object& obj = objects.front();
    if(obj.name == "constant_texture")
    {
        Spectrum color;
        if(isalpha(obj.property.front().name[0]))
        {
            color = get_color(obj.property.front().name);
        }
        else
        {
            // TODO: rgb
        }
        texture = std::make_shared<constant_texture<Spectrum>>(color);
    }
    else if(obj.name == "checker_texture")
    {
        usize width, height;
        Spectrum c1, c2;
        width = std::atoi(obj.property[0].name.data());
        height = std::atoi(obj.property[1].name.data());
        c1 = get_color(obj.property[2].name);
        c2 = get_color(obj.property[3].name);
        texture = std::make_shared<checker_texture<Spectrum>>
        (
            width, height, c1, c2
        );
    }
    return texture;
}

// std::shared_ptr<Material> get_material(const std::vector<Object>& objects)
// {
//     std::shared_ptr<Material> material;
//     const Object& obj = objects.front();
//     if(obj.name == "Matte")
//     {
//         auto texture = get_texture(obj.property);
//         material = std::make_shared<Matte>(std::move(texture));
//     }
//     return material;
// }

std::tuple
<
    Scene,
    perspective_camera,
    std::unique_ptr<Integrator>
>
parse(const std::string_view& filename)
{
    auto result = Parser::parse(filename);
    if(result.is_err())
    {
        println(result.err_value());
        exit(-1);
    }
    Scene scene;
    std::optional<perspective_camera> camera;
    std::unique_ptr<Integrator> integrator;

    for(const auto& object : result.ok_value())
    {
        if(object.name == "perspective_camera")
        {
            usize width, height;
            Point3f pos, at;
            Vector3f up;
            f32 fov;
            for(const auto& property : object.property)
            {
                if(property.name == "width")
                {
                    width = get<usize, 1>(property.property);
                }
                else if(property.name == "height")
                {
                    height = get<usize, 1>(property.property);
                }
                else if(property.name == "pos")
                {
                    pos = get<Point3f, 3>(property.property);
                }
                else if(property.name == "at")
                {
                    at = get<Point3f, 3>(property.property);
                }
                else if(property.name == "up")
                {
                    up = get<Vector3f, 3>(property.property);
                }
                else if(property.name == "fov")
                {
                    fov = get<f32, 1>(property.property);
                }
            }
            camera = perspective_camera(width, height, pos, at, up, fov);
        }
        else if(object.name == "direct_light_integrator")
        {
            integrator = std::make_unique<direct_light_integrator>();
        }
        else if(object.name == "path_tracing_integrator")
        {
            integrator = std::make_unique<path_tracing_integrator>();
        }
        else if(object.name == "point_light")
        {
            Point3f position;
            Spectrum intensity;
            for(const auto& property : object.property)
            {
                if(property.name == "position")
                {
                    position = get<Point3f, 3>(property.property);
                }
                else if(property.name == "intensity")
                {
                    intensity = Spectrum(std::atof
                    (
                        property.property.front().name.data()
                    ));
                }
            }
            scene.add(std::make_shared<point_light>(position, intensity));
        }
        else if(object.name == "Plane")
        {
            f32 length;
            Matrix4f transform = Matrix4f::identity();
            std::shared_ptr<Material> material;
            for(const auto property : object.property)
            {
                if(property.name == "length")
                {
                    length = get<f32, 1>(property.property);
                }
                else if(property.name == "Transform")
                {
                    transform = get_transform(property.property);
                }
                else if(property.name == "Matte")
                {
                    material = std::make_shared<Matte>(get_texture(property.property));
                }
            }
            if(material)
            {
                if(transform == Matrix4f::identity())
                {
                    const auto& m = scene.identity();
                    scene.add
                    (
                        std::make_shared<geometry_primitive>
                        (
                            std::make_shared<Plane>(m, m, length),
                            material
                        )
                    );
                }
                else
                {
                    const auto& [m, inv] = scene.add(transform);
                    scene.add
                    (
                        std::make_shared<geometry_primitive>
                        (
                            std::make_shared<Plane>(m, inv, length),
                            material
                        )
                    );
                }
            }
        }
    }
 
    if(scene.lights.empty() || scene.primitives.empty() || !integrator)
    {
        println("without light/primitive/integrator");
        exit(-1);
    }

    return std::make_tuple
    (
        std::move(scene),
        std::move(camera.value()),
        std::move(integrator)
    );
}

// void make_sphere(Scene& scene, const Vector3f& translate, f32 radius
//     , std::shared_ptr<Material> material)
// {
//     if(translate == Vector3f{0})
//     {
//         const auto& m = scene.identity();
//         scene.add(std::make_shared<geometry_primitive>(std::make_shared<Sphere>(m, m, radius), material));
//     }
//     else
//     {
//         const auto& [m, inv] = scene.add
//         (
//             Transform<f32>::translate(translate),
//             Transform<f32>::translate(-translate)
//         );
//         scene.add(std::make_shared<geometry_primitive>(std::make_shared<Sphere>(m, inv, radius), material));
//     }
// }

// void make_plane(Scene& scene, const Matrix4f& transform, f32 length
//     , std::shared_ptr<Material> material)
// {
//     if(transform == Matrix4f::identity())
//     {
//         const auto& m = scene.identity();
//         scene.add(std::make_shared<geometry_primitive>(std::make_shared<Plane>(m, m, length), material));
//     }
//     else
//     {
//         const auto& [m, inv] = scene.add(transform);
//         scene.add(std::make_shared<geometry_primitive>(std::make_shared<Plane>(m, inv, length), material));
//     }
// }

// template <Axis axis>
// Matrix4f translate_rotate_scale(const Vector3f& t, f32 s, f32 angle)
// {
//     return Transform<f32>::translate(t) * Transform<f32>::rotate<axis>(angle) * Transform<f32>::scale(s);
// }

// template <Axis axis>
// Matrix4f translate_rotate(const Vector3f& t, f32 angle)
// {
//     return Transform<f32>::translate(t) * Transform<f32>::rotate<axis>(angle);
// }

// template <typename T, typename... Args>
// std::shared_ptr<Material> make_matte(Args... args)
// {
//     return std::make_shared<Matte>(std::make_shared<T>(std::forward<Args>(args)...));
// }

// std::tuple<Scene, perspective_camera, std::unique_ptr<Integrator>>
// make_scene(usize width, usize height)
// {
//     auto camera = perspective_camera
//     {
//         width, height,
//         Point3f{0, 5, 10}, Point3f{0, 0, 0}, Vector3f{0, 1, 0},
//         60
//     };

//     Scene scene;

//     constexpr f32 length = 10;

//     auto m_back = translate_rotate<Axis::X>({0, length, -length}, 90);
//     auto m_left = translate_rotate<Axis::Z>({-length, length, 0}, 270);
//     auto m_right = translate_rotate<Axis::Z>({length, length, 0}, 90);
//     // auto m_ceil = Transform<f32>::translate({0, length, 0});

//     make_plane(scene, Matrix4f::identity(), length, make_matte<checker_texture<Spectrum>>(4, 4, RGB::white, RGB::cyan));
//     make_plane(scene, m_back, length, make_matte<constant_texture<Spectrum>>(RGB::white));
//     make_plane(scene, m_left, length, make_matte<constant_texture<Spectrum>>(RGB::red));
//     make_plane(scene, m_right, length, make_matte<constant_texture<Spectrum>>(RGB::green));
//     // make_plane(scene, m_ceil, length, make_matte<constant_texture<Spectrum>>(white));

//     auto m_cube = Transform<f32>::translate({0, 1, 3});
//     const auto& [m1, m2] = scene.add(m_cube);
//     auto cube = std::make_shared<Cube>(m1, m2);
//     scene.add(std::make_shared<geometry_primitive>(cube, make_matte<constant_texture<Spectrum>>(RGB::dodger_blue)));

//     make_sphere(scene, {0, 4, -3}, 1, std::make_shared<Mirror>());
//     make_sphere(scene, {3, 1, -3}, 1, make_matte<constant_texture<Spectrum>>(RGB::yellow));
//     make_sphere(scene, {-3, 1, -3}, 1, make_matte<checker_texture<Spectrum>>(20, 20, RGB::white, RGB::black));

//     scene.add(std::make_shared<point_light>(Point3f{0, 10, 0}, Spectrum{100}));
//     Timer::elapsed(&scene, &Scene::build);

//     return std::make_tuple
//     (
//         std::move(scene), std::move(camera),
//         std::make_unique<path_tracing_integrator>()
//     );
// }
