#include <Core/BxDF/BxDF.hpp>

BxDF::BxDF(bxdf_type type) : type(type) {}

bool BxDF::is_specular(bxdf_type type)
{
    return static_cast<int>(type) & static_cast<int>(bxdf_type::Specular);
}

Vector3f BxDF::reflect(const Vector3f& w) const
{
    return {-w.x, -w.y, w.z};
}
