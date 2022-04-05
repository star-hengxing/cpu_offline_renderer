#include <Scene/Scene.hpp>

#include <Hinae/Transform.hpp>

#include <Core/Primitive/geometry_primitive.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Material/Material.hpp>
#include <Core/Shape/Triangle.hpp>
#include <Core/Light/Light.hpp>
#include <hit_record.hpp>
#include <util/util.hpp>

const Matrix4f& transform_vector::identity()
{
    return identity_matrix;
}

std::tuple<const Matrix4f&, const Matrix4f&>
transform_vector::add(const Matrix4f& transform, const Matrix4f& inv)
{
    matrix.emplace_back(std::make_unique<std::tuple<Matrix4f, Matrix4f>>(transform, inv));
    const auto& [m1, m2] = *matrix.back().get();
    return {m1, m2};
}

Scene::Scene(primitive_vector_type&& primitives, light_vector_type&& lights)
{
    this->primitives = primitive_vector_type(primitives);
    this->lights = light_vector_type(lights);
}

Scene& Scene::add(std::shared_ptr<Light> light)
{
    lights.emplace_back(std::move(light));
    return *this;
}

std::tuple<const Matrix4f&, const Matrix4f&>
Scene::add(const Matrix4f& transform)
{
    return transforms.add(transform, transform.inverse());
}

std::tuple<const Matrix4f&, const Matrix4f&>
Scene::add(const Matrix4f& transform, const Matrix4f& inv)
{
    return transforms.add(transform, inv);
}

const Matrix4f& Scene::identity() const
{
    return transform_vector::identity();
}

Scene& Scene::add(std::shared_ptr<geometry_primitive> primitive)
{
    primitives.emplace_back(std::move(primitive));
    return *this;
}

Scene& Scene::add(
    const std::string_view& path,
    const Matrix4f& transform,
    std::shared_ptr<Material> material)
{
    std::optional<Mesh> mesh = Mesh::read(path);
    if(!mesh) exit(-1);
    mesh->info();

    auto triangle_mesh = std::make_shared<TriangleMesh>
    (
        transform,
        mesh->faces,
        mesh->vertices,
        mesh->normals,
        mesh->uvs
    );

    Bounds3f bounds{Point3f{0}}; // 看看模型有多大
    const usize current = primitives.size();
    const usize add_size = mesh->faces.size();

    primitives.reserve(current + add_size);
    for(auto i : range(add_size))
    {
        primitives.emplace_back(std::make_shared<geometry_primitive>
        (
            std::make_shared<Triangle>
            (
                identity(), identity(),
                triangle_mesh, triangle_mesh->faces[i]
            ),
            material
        ));
        bounds = Union(bounds, primitives.back()->world_bound());
    }

    println(bounds);
    return *this;
}

Scene& Scene::build()
{
    bvh.build(primitives);
    return *this;
}

std::optional<hit_record> Scene::intersect(const Ray3f &ray) const
{
    hit_record record;
    std::shared_ptr<geometry_primitive> primitive;

    for(const auto& v : primitives)
    {
        if(v->intersect(ray, record))
            primitive = v;
    }

    if(primitive)
    {
        primitive->get_intersect_record(ray, record);
        primitive->compute_BxDF(record);
        return std::make_optional(record);
    }
    else
        return {};
}

bool Scene::intersect_p(const Ray3f& shadow_ray) const
{
    hit_record record;
    for(const auto& primitive : primitives)
    {
        if(primitive->intersect(shadow_ray, record))
            return true;
    }
    return false;
}

std::optional<hit_record> Scene::bvh_intersect(const Ray3f &ray) const
{
    return bvh.intersect(ray);
}

bool Scene::bvh_intersect_p(const Ray3f& shadow_ray) const
{
    return bvh.intersect_p(shadow_ray);
}
