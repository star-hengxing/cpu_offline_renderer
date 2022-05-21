#pragma once

#include <Core/Camera/perspective_camera.hpp>
#include <Core/Integrator/Integrator.hpp>
#include <Scene/Scene.hpp>

using return_type = std::tuple
<
    Scene,
    perspective_camera,
    std::unique_ptr<Integrator>
>;

return_type cornell_box();

return_type native();
