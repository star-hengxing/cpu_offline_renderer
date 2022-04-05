#include <Core/Camera/perspective_camera.hpp>

#include <Hinae/Transform.hpp>

perspective_camera::perspective_camera(usize width, usize height
    , const Point3f& pos
    , const Point3f& at
    , const Vector3f& up
    , f32 fov_angle)
    : Camera(width, height, pos, at, up)
    , fov_radian(std::tan(to_radian(fov_angle / 2))) {}

Ray3f perspective_camera::generate_ray(const Point2f& pixel) const
{
    Point2f p = to_ndc(pixel);
    const Vector3f direction
    {
        p.x * fov_radian,
        p.y * fov_radian,
        -1
    };
    return
    {
        pos,
        look_at() * direction.normalized()
    };
}
