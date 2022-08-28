#pragma once

#include <iostream>
#include <memory.h>

#include <util/basic_type.hpp>

inline void bar(f64 f)
{
    fprintf(stderr, "Rendering...%d%%\r", static_cast<int>(f * 100));
}

struct Integrator;
struct Config;
struct Camera;
struct Scene;

std::unique_ptr<Integrator> make_integrator(const Config& config);

void render(const char* path);

void render_single_image(const Config& config, const Scene& scene, Camera& camera);
