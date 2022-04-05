#include <Core/Primitive/geometry_primitive.hpp>

#include <Core/Material/Material.hpp>
#include <Core/Shape/Shape.hpp>
#include <hit_record.hpp>

geometry_primitive::geometry_primitive(
    std::shared_ptr<Shape> shape
    , std::shared_ptr<Material> material)
    : shape(shape), material(material) {}

bool geometry_primitive::intersect(const Ray3f& ray3, hit_record& record) const
{
    return shape->intersect(ray3, record);
}

Bounds3f geometry_primitive::world_bound() const
{
    return shape->world_bound();
}

void geometry_primitive::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    shape->get_intersect_record(ray3, record);
}

void geometry_primitive::compute_BxDF(hit_record& record) const
{
    if(material)
    {
        material->compute(record);
    }
}
