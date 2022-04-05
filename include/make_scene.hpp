#pragma once

#include <string_view>

#include <Core/Camera/perspective_camera.hpp>
#include <Core/Integrator/Integrator.hpp>
#include <Scene/Scene.hpp>


std::tuple
<
    Scene,
    perspective_camera,
    std::unique_ptr<Integrator>
>
parse(const std::string_view& filename);
