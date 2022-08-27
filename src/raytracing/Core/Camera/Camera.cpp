#include <raytracing/Core/Camera/Camera.hpp>

#include <math/Transform.hpp>

Camera::Camera(usize width, usize height
    , const Point3f& pos
    , const Point3f& at
    , const Vector3f& up)
    : pos(pos), at(at), up(up), photo(width, height)
{
    const auto aspect = static_cast<f32>(width) / static_cast<f32>(height);
    aspect_ratio_x = max(1.0f, aspect);
    aspect_ratio_y = min(1.0f, aspect);
}

Matrix4f Camera::look_at() const
{
    return Transform<f32>::look_at(pos, at, up);
}

void Camera::save(const char* path) const
{
    ::save(photo, path);
}

Point2f Camera::to_ndc(const Point2f& pixel) const
{
    const auto [width, height] = photo.get_width_height();
    return
    {
        (pixel.x / static_cast<f32>(width) * 2 - 1) * aspect_ratio_x,
        (1 - pixel.y / static_cast<f32>(height) * 2) * aspect_ratio_y
    };
}
