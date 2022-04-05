#pragma once

#include "Camera.hpp"

struct perspective_camera : public Camera
{
private:
    f32 fov_radian;

public:
    perspective_camera(usize width, usize height
        , const Point3f& pos
        , const Point3f& at
        , const Vector3f& up
        , f32 fov_angle);

    virtual Ray3f generate_ray(const Point2f& pixel) const override;
};
