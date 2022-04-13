#include <Core/Light/spot_light.hpp>

#include <hit_record.hpp>

spot_light::spot_light(const Point3f& position
        , const Vector3f& intensity
        , f32 cos_width, f32 cos_falloff
        , const Vector3f& dir)
    : point_light(position, intensity)
    , cos_width(std::cos(to_radian(cos_width)))
    , cos_falloff(std::cos(to_radian(cos_falloff)))
    , dir(dir) {}

Spectrum spot_light::Li(const hit_record& record, const Vector3f& w) const
{
    const Vector3f direction = position - record.p;
    return intensity * falloff(-direction.normalized()) / direction.norm2();
}

f32 spot_light::falloff(const Vector3f& v) const
{
    f32 cos_theta = dot(v, dir);
    if(cos_theta < cos_width)   return 0;
    if(cos_theta > cos_falloff) return 1;
    return pow4((cos_theta - cos_width) / (cos_falloff - cos_width));
}
