#include <Core/Shape/Triangle.hpp>

#include <Hinae/Transform.hpp>

#include <Core/Mesh/TriangleMesh.hpp>
#include <hit_record.hpp>

Triangle::Triangle(const Matrix4f& local_to_world, const Matrix4f& world_to_local,
    const std::shared_ptr<TriangleMesh>& mesh, const Face& face)
        : Shape(local_to_world, world_to_local)
        , mesh(mesh)
        , face(face) {}

#define GET_DATA(data, type) {         \
    mesh->data[face.vertices[0].type], \
    mesh->data[face.vertices[1].type], \
    mesh->data[face.vertices[2].type]  \
}

template <typename T>
std::tuple<const T&, const T&, const T&>
get(const std::shared_ptr<TriangleMesh>& mesh, const Face& face)
{
    if constexpr(std::is_same_v<T, Point3f>)
    {
        return GET_DATA(vertices, point);
    }
    else if constexpr(std::is_same_v<T, Point2f>)
    {
        return GET_DATA(uvs, uv);
    }
    else
    {
        static_assert(std::is_same_v<T, Vector3f>);
        return GET_DATA(normals, normal);
    }
}

Bounds3f Triangle::world_bound() const
{
    const auto& [p1, p2, p3] = get<Point3f>(mesh, face);
    return Union(Bounds3f{p1, p2}, p3); 
}

Bounds3f Triangle::local_bound() const
{
    const auto& [p1, p2, p3] = get<Point3f>(mesh, face);
    return Union(Bounds3f{world_to_local * p1, world_to_local * p2}
        , world_to_local * p3); 
}

bool Triangle::intersect(const Ray3f& ray3, hit_record& record) const
{
    const auto& [p1, p2, p3] = get<Point3f>(mesh, face);

    const Vector3f E1 = p2 - p1;
    const Vector3f E2 = p3 - p1;
    const Vector3f S  = ray3.origin - p1;
    const Vector3f S1 = cross(ray3.direction, E2);
    const Vector3f S2 = cross(S, E1);

    const f32 inv = reciprocal(dot(S1, E1));

    const f32 t = dot(S2, E2) * inv;

    f32 beta  = dot(S1, S) * inv;
    f32 gamma = dot(S2, ray3.direction) * inv;

    if(beta < 0 || beta > 1 || gamma < 0 || (beta + gamma) > 1)
        return false;

    f32 alpha = 1 - beta - gamma;
    record.p = {alpha, beta, gamma};
    return record.set_t(t);
}

void Triangle::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    const auto [alpha, beta, gamma] = record.p;
    if(mesh->normals)
    {
        const auto& [n1, n2, n3] = get<Vector3f>(mesh, face);
        record.n = alpha * n1 + beta * n2 + gamma * n3;
    }

    if(mesh->uvs)
    {
        const auto& [uv1, uv2, uv3] = get<Point2f>(mesh, face);
        record.uv = as<Point2, f32>(
            as<Vector2, f32>(uv1) * alpha
            + as<Vector2, f32>(uv2) * beta
            + as<Vector2, f32>(uv3) * gamma);
    }

    record.p = ray3.at(record.t_min);
}
