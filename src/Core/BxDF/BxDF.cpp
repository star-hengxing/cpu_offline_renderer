#include <Core/BxDF/BxDF.hpp>

BxDF::BxDF(Type type) : type(type) {}

bool BxDF::is_specular(Type type)
{
    return type & Type::Specular;
}
