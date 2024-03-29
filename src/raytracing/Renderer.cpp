
#include <filesystem>

#include <raytracing/Core/Sampler/Sampler.hpp>
#include <raytracing/Renderer.hpp>
#include <util/Color/color_space.hpp>
#include <util/render_queue.hpp>
#include <util/util.hpp>

static void bar(f64 f)
{
    fprintf(stderr, "Rendering...%d%%\r", static_cast<int>(f * 100));
}

Renderer::Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator)
    : scene(std::move(scene)), camera(std::move(camera)), integrator(std::move(integrator)) {}

using namespace std::literals;

static auto spp = usize{4};
static auto thread_count = usize{std::jthread::hardware_concurrency()};

Result<Renderer, std::string_view> Renderer::init(int argc, char const *argv[], return_type (*fn)())
{
    if(argc >= 2)
    {
        spp = std::atoi(argv[1]);
        if(argc == 3)
        {
            const auto tmp = std::atoi(argv[2]);
            thread_count = min<usize>(thread_count, tmp);
        }
    }

    auto [scene, camera, integrator] = (fn == nullptr ? native() : fn());

    println("BVH build...");
    Timer::elapsed(&scene, &Scene::build);

    Renderer render(std::move(scene), std::move(camera), std::move(integrator));
    return Ok(std::move(render));
}

void Renderer::render()
{
    const auto [w, h] = camera.photo.get_width_height();
    const auto width = w;
    const auto height = h;
    println("rendering...");

    const auto for_width = [&](usize y, Sampler &sampler)
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
    };

    Timer timer;
    if(thread_count == 0)
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
        render_queue{thread_count, height, [&](usize y)
        {
            static thread_local Sampler sampler;
            for_width(y, sampler);
            std::scoped_lock lock(m);
            bar(static_cast<f64>(y) / height);
        }};
    }

    timer.elapsed();
    std::filesystem::create_directory("images");
    camera.save_photo("images/dev.png");
}
