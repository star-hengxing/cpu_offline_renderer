#pragma once

#include <Core/Camera/perspective_camera.hpp>
#include <Core/Integrator/Integrator.hpp>
#include <Scene/Scene.hpp>
#include <util/Result.hpp>

enum class Error
{
    invalid_parameter,
    parse_failed,
};

std::ostream& operator << (std::ostream& os, Error error);

struct Renderer
{
private:
    Scene scene;
    perspective_camera camera;
    std::unique_ptr<Integrator> integrator;

    Renderer(Scene&& scene, perspective_camera&& camera, std::unique_ptr<Integrator>&& integrator);

public:
    static Result<Renderer, Error> init(int argc, char const *argv[]);

    void render();
};
