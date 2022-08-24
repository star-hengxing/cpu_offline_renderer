#pragma once

#include <raytracing/Core/Camera/perspective_camera.hpp>
#include <raytracing/Core/Integrator/Integrator.hpp>
#include <raytracing/Scene/Scene.hpp>
#include <util/Result.hpp>
#include <raytracing/make_scene.hpp>

struct Renderer
{
private:
    Scene scene;
    perspective_camera camera;
    std::unique_ptr<Integrator> integrator;

    Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator);

public:
    static Result<Renderer, std::string_view> init(int argc, char const *argv[], return_type (*fn)());

    void render();
};
