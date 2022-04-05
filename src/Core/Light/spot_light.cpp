#include <Core/Light/spot_light.hpp>

spot_light::spot_light(const Point3f& position
        , const Vector3f& intensity
        , f32 cos_width, f32 cos_falloff
        , const Vector3f& dir)
    : point_light(position, intensity)
    , cos_width(std::cos(to_radian(cos_width)))
    , cos_falloff(std::cos(to_radian(cos_falloff)))
    , dir(dir) {}

Spectrum spot_light::Li(const Point3f& p) const
{
    const Vector3f direction = position - p;
    return intensity * falloff(-direction.normalized()) / direction.norm2();
}

std::tuple<Vector3f, f32> spot_light::get_dir_and_distance(const Point3f& p) const
{
    return point_light::get_dir_and_distance(p);   
}

f32 spot_light::falloff(const Vector3f& v) const
{
    f32 cos_theta = dot(v, dir);
    if(cos_theta < cos_width)   return 0;
    if(cos_theta > cos_falloff) return 1;
    return pow4((cos_theta - cos_width) / (cos_falloff - cos_width));
}
