#include <hit_record.hpp>

#include <global.hpp>

bool hit_record::is_hit() const
{
    return t_min < INFINITY_<f32>;
}

bool hit_record::is_light() const
{
    return light != nullptr;
}

bool hit_record::set_t(f32 t)
{
    if(t >= t_min || t <= 0.001f || t >= t_max)
        return false;

    t_min = t;
    return true;
}

Ray3f hit_record::new_ray(const Vector3f& direction) const
{
    return {Offset::robust(p, flip_normal(direction, n)), direction};
}
