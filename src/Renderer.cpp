#include <Renderer.hpp>

#include <util/Color/color_space.hpp>
#include <Core/Sampler/Sampler.hpp>
#include <util/render_queue.hpp>
#include <make_scene.hpp>
#include <util/util.hpp>

static void bar(f64 f)
{
    fprintf(stderr, "Rendering...%d%%\r", static_cast<int>(f * 100));
}

Renderer::Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator)
    : scene(std::move(scene)), camera(std::move(camera)), integrator(std::move(integrator)) {}

using namespace std::literals;

usize spp = 4;
usize thread_count = 0;

Result<Renderer, std::string_view> Renderer::init(int argc, char const *argv[])
{
    if(argc >= 2)
    {
        spp = std::atoi(argv[1]);
        if(argc == 3)
            thread_count = std::atoi(argv[2]);
    }

    auto [scene, camera, integrator] = parse();

    println("BVH build...");
    Timer::elapsed(&scene, &Scene::build);

    Renderer render(std::move(scene), std::move(camera), std::move(integrator));
    return Ok(std::move(render));
}

void Renderer::render()
{
    const auto [width, height] = camera.photo.get_width_height();
    println("rendering...");

    Timer timer;
    if(thread_count == 0)
    {
        Sampler sampler;

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
                    return integrator->Li(ray, scene, sampler);
                });
                camera.photo[x][y] = color_space(radiance).to_srgb();
            }
            bar(static_cast<f64>(y) / height);
        }
    }
    else
    {
        std::mutex m;
        const auto count = min(thread_count, static_cast<usize>(std::jthread::hardware_concurrency()));
        render_queue{count, height, [&](usize y)
        {
            static thread_local Sampler sampler;
            for(usize x : range(width))
            {
                Spectrum radiance = arithmetic_mean(spp, [&]()
                {
                    Point2f pixel = sampler.get_2D();
                    pixel.x += x;
                    pixel.y += y;
                    Ray3f ray = camera.generate_ray(pixel);
                    return integrator->Li(ray, scene, sampler);
                });
                camera.photo[x][y] = color_space(radiance).to_srgb();
            }
            std::scoped_lock lock(m);
            bar(static_cast<f64>(y) / height);
        }};
    }

    timer.elapsed();
    camera.save_photo("images/dev.png");
}
