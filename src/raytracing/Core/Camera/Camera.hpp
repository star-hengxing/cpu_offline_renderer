#pragma once

#include <math/Matrix4.hpp>
#include <math/Vector3.hpp>
#include <math/Point3.hpp>
#include <math/Point2.hpp>
#include <math/Ray3.hpp>

#include <util/Image.hpp>

struct Camera
{
protected:
    f32 aspect_ratio_x, aspect_ratio_y;
    // f32 z_near, z_far;
    Point3f pos, at;
    Vector3f up;

public:
    Image photo;

public:
    Camera(usize width, usize height
        , const Point3f& pos
        , const Point3f& at
        , const Vector3f& up);

    // virtual ~Camera() = default;

    void save_photo(const std::string_view& filename) const;

    Matrix4f look_at() const;

    Point2f to_ndc(const Point2f& pixel) const;

    virtual Ray3f generate_ray(const Point2f& pixel) const = 0;
};
