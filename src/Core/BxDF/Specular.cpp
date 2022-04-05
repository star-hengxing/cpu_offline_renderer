#include <Core/BxDF/Specular.hpp>

#include <Hinae/physics.hpp>

Specular::Specular(const Spectrum& R)
    : BxDF(Type(Type::Specular | Type::Reflection)), R(R) {}

Spectrum Specular::f(const Vector3f& wi, const Vector3f& wo) const
{
    return {0};
}

f32 Specular::pdf(const Vector3f& wi, const Vector3f& wo) const
{
    return 0;
}

std::tuple<Spectrum, Vector3f, f32>
Specular::sample_f(const Vector3f& wi, const Vector3f& n, const Point2f& p) const
{
    const auto wo = reflect(-wi, n);
    return {Spectrum{1}, wo, 1};
}
