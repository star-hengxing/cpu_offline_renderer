#include <filesystem>

#include <raytracing/Scene/scene_payload.hpp>
#include <raytracing/Scene/Scene.hpp>

#include <raytracing/Core/Integrator/ambient_occlusion_integrator.hpp>
#include <raytracing/Core/Integrator/direct_light_integrator.hpp>
#include <raytracing/Core/Integrator/path_tracing_integrator.hpp>

#include <raytracing/Core/Sampler/Sampler.hpp>
#include <raytracing/Renderer/Renderer.hpp>

#include <util/Color/color_space.hpp>
#include <util/render_queue.hpp>
#include <util/range.hpp>
#include <util/marco.hpp>
#include <util/Timer.hpp>
#include <util/util.hpp>
#include <util/io.hpp>

#include <math/basic.hpp>

std::unique_ptr<Integrator> make_integrator(const Config& config)
{
    switch (config.type)
    {
        using enum integrator_type;

        case ao:
        {
            return std::make_unique<ambient_occlusion_integrator>(config.spp);
        }

        case direct:
        {
            return std::make_unique<direct_light_integrator>();
        }

        case pt:
        {
            return std::make_unique<path_tracing_integrator>();
        }
    }
}

void render(const char* path)
{
    scene_payload payload;
    if (!payload.load(path))
        return;

    auto scene = payload.get_scene();

    println("BVH build...");
    Timer::elapsed(&scene, &Scene::build);
    println("rendering...");

    render_single_image(payload.config, scene, payload.camera);

    payload.clean();
}

void render_single_image(const Config& config, const Scene& scene, Camera& camera)
{
    const auto integrator = make_integrator(config);
    const auto [width, height] = camera.photo.get_width_height();
    const auto for_width = [&](usize y, Sampler &sampler)
    {
        for(usize x : range(width))
        {
            Spectrum radiance = arithmetic_mean(config.spp, [&]()
            {
                Point2f pixel = sampler.get_2D();
                pixel.x += x;
                pixel.y += y;
                Ray3f ray = camera.generate_ray(pixel);
                return integrator->Li(ray, scene, sampler);
            });
            camera.photo[x][y] = color_space(radiance).to_srgb();
        }
    };

    Timer timer;
    if(config.thread == 0)
    {
        Sampler sampler;
        for(usize y : range(height))
        {
            for_width(y, sampler);
            bar(static_cast<f64>(y) / height);
        }
    }
    else
    {
        std::mutex m;
        const auto count = min(config.thread, static_cast<u32>(std::jthread::hardware_concurrency()));
        render_queue{count, height, [&](usize y)
        {
            static thread_local Sampler sampler;
            for_width(y, sampler);
            std::scoped_lock lock(m);
            bar(static_cast<f64>(y) / height);
        }};
    }

    timer.elapsed();
    // mkdir -p
    std::filesystem::create_directory("images");
    camera.save("images/dev.png");
}
