#pragma once

#include <Core/Camera/perspective_camera.hpp>
#include <Core/Integrator/Integrator.hpp>
#include <Scene/Scene.hpp>
#include <util/Result.hpp>

struct Renderer
{
private:
    Scene scene;
    perspective_camera camera;
    std::unique_ptr<Integrator> integrator;

    Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator);

public:
    static Result<Renderer, std::string_view> init(int argc, char const *argv[]);

    void render();
};
