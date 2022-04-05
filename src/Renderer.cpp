#include <Renderer.hpp>

#include <util/Color/color_space.hpp>
#include <Core/Sampler/Sampler.hpp>
#include <make_scene.hpp>
#include <util/util.hpp>

static void bar(f64 f)
{
    fprintf(stderr, "Rendering...%d%%\r", static_cast<int>(f * 100));
}

std::ostream& operator << (std::ostream& os, Error error)
{
    switch(error)
    {
        using enum Error;
        case invalid_parameter: os << "invalid parameter"; break;
        case parse_failed: os << "parse failed"; break;
    }
    return os;
}

Renderer::Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator)
    : scene(std::move(scene)), camera(std::move(camera)), integrator(std::move(integrator)) {}

Result<Renderer, Error> Renderer::init(int argc, char const *argv[])
{
    if(argc != 2) return Err(Error::invalid_parameter);

    println("Scene file parsing...");
    Timer timer;
    auto [scene, camera, integrator] = parse(argv[1]);
    timer.elapsed();

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
