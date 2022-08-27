#include <raytracing/Core/Primitive/geometry_primitive.hpp>

#include <raytracing/Core/Material/Material.hpp>
#include <raytracing/Core/Shape/Shape.hpp>
#include <raytracing/hit_record.hpp>

geometry_primitive::geometry_primitive(Shape* shape, Material* material, Light* light) :
    shape(shape), material(material), light(light) {}

bool geometry_primitive::intersect(const Ray3f& ray3, hit_record& record) const
{
    if (!shape->intersect(ray3, record))
        return false;

    if (light)
    {
        record.light = light;
    }
    return true;
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
    if (material)
    {
        material->compute(record);
    }
}
