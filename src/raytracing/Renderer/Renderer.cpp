#include <filesystem>

#include <raytracing/Scene/scene_payload.hpp>
#include <raytracing/Scene/Scene.hpp>

#include <raytracing/Core/Sampler/Sampler.hpp>
#include <util/Color/color_space.hpp>
#include <util/render_queue.hpp>
#include <util/util.hpp>

#include <util/range.hpp>
#include <util/marco.hpp>
#include <util/Timer.hpp>
#include <util/io.hpp>

void render(const char* path)
{
    scene_payload payload;
    if (!payload.load(path))
        return;

    auto scene = payload.get_scene();

    println("BVH build...");
    Timer::elapsed(&scene, &Scene::build);
}

// static usize spp = 4;
// static usize thread_count = 0;

// void Renderer::render()
// {
//     const auto [width, height] = camera.photo.get_width_height();
//     println("rendering...");

//     const auto for_width = [&](usize y, Sampler &sampler)
//     {
//         for(usize x : range(width))
//         {
//             Spectrum radiance = arithmetic_mean(spp, [&]()
//             {
//                 Point2f pixel = sampler.get_2D();
//                 pixel.x += x;
//                 pixel.y += y;
//                 Ray3f ray = camera.generate_ray(pixel);
//                 return integrator->Li(ray, scene, sampler);
//             });
//             camera.photo[x][y] = color_space(radiance).to_srgb();
//         }
//     };

//     Timer timer;
//     if(thread_count == 0)
//     {
//         Sampler sampler;
//         for(usize y : range(height))
//         {
//             for_width(y, sampler);
//             bar(static_cast<f64>(y) / height);
//         }
//     }
//     else
//     {
//         std::mutex m;
//         const auto count = min(thread_count, static_cast<usize>(std::jthread::hardware_concurrency()));
//         render_queue{count, height, [&](usize y)
//         {
//             static thread_local Sampler sampler;
//             for_width(y, sampler);
//             std::scoped_lock lock(m);
//             bar(static_cast<f64>(y) / height);
//         }};
//     }

//     timer.elapsed();
//     // mkdir -p
//     std::filesystem::create_directory("images");
//     camera.save_photo("images/dev.png");
// }
