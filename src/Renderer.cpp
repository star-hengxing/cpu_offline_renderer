#include <Renderer.hpp>

#include <util/Color/color_space.hpp>
#include <Core/Sampler/Sampler.hpp>
#include <make_scene.hpp>
#include <util/util.hpp>

static void bar(f64 f)
{
    fprintf(stderr, "Rendering...%d%%\r", static_cast<int>(f * 100));
}

Renderer::Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator)
    : scene(std::move(scene)), camera(std::move(camera)), integrator(std::move(integrator)) {}

using namespace std::literals;

Result<Renderer, std::string_view> Renderer::init(int argc, char const *argv[])
{
    // if(argc != 2) return Err("invalid parameter"sv);

    auto [scene, camera, integrator] = parse();

    println("BVH build...");
    Timer::elapsed(&scene, &Scene::build);

    Renderer render(std::move(scene), std::move(camera), std::move(integrator));
    return Ok(std::move(render));
}

void Renderer::render()
{
    const usize spp = 4;
    const auto [width, height] = camera.photo.get_width_height();
    thread_local Sampler sampler;

    println("rendering...");
    Timer::elapsed([&]()
    {
        for(usize y : range(height))
        {
            for(usize x : range(width))
            {
                Spectrum radiance = arithmetic_mean(spp, [&]()
                {
                    Point2f pixel = sampler.get_2D();
                    pixel.x += x;
                    pixel.y += y;
                    Ray3f ray = camera.generate_ray(pixel);
                    return clamp(0.0f, integrator->Li(ray, scene, sampler), 1.0f);
                });
                camera.photo[x][y] = color_space(radiance).to_srgb();
            }
            bar(static_cast<f64>(y) / height);
        }
    });

    camera.save_photo("images/dev.png");
}
