#include <raytracing/Core/Shape/Triangle.hpp>

#include <math/Transform.hpp>

#include <raytracing/Core/Mesh/TriangleMesh.hpp>
#include <raytracing/hit_record.hpp>

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

static Point3f interpolate(f32 alpha, f32 beta, f32 gamma,
    const Point3f& a, const Point3f& b, const Point3f& c)
{
    return as<Point3, f32>
    (
        as<Vector3, f32>(a) * alpha +
        as<Vector3, f32>(b) * beta +
        as<Vector3, f32>(c) * gamma
    );
}

static Point2f interpolate(f32 alpha, f32 beta, f32 gamma,
    const Point2f& a, const Point2f& b, const Point2f& c)
{
    return as<Point2, f32>
    (
        as<Vector2, f32>(a) * alpha +
        as<Vector2, f32>(b) * beta +
        as<Vector2, f32>(c) * gamma
    );
}

static Vector3f interpolate(f32 alpha, f32 beta, f32 gamma,
    const Vector3f& a, const Vector3f& b, const Vector3f& c)
{
    return alpha * a + beta * b + gamma * c;
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

    const auto inv = reciprocal(dot(S1, E1));

    const auto t = dot(S2, E2) * inv;

    const auto beta  = dot(S1, S) * inv;
    const auto gamma = dot(S2, ray3.direction) * inv;

    if(beta < 0 || beta > 1 || gamma < 0 || (beta + gamma) > 1)
        return false;

    const auto alpha = 1 - beta - gamma;

    const auto updated = record.set_t(t);
    if(updated) record.p = {alpha, beta, gamma};
    
    return updated;
}

void Triangle::get_intersect_record(const Ray3f& ray3, hit_record& record) const
{
    const auto [alpha, beta, gamma] = record.p;
    if(mesh->normals)
    {
        const auto& [n1, n2, n3] = get<Vector3f>(mesh, face);
        record.n = interpolate(alpha, beta, gamma, n1, n2, n3);
    }

    if(mesh->uvs)
    {
        const auto& [uv1, uv2, uv3] = get<Point2f>(mesh, face);
        record.uv = interpolate(alpha, beta, gamma, uv1, uv2, uv3);
    }

    record.p = ray3.at(record.t_min);
}

f32 Triangle::area() const
{
    const auto& [p1, p2, p3] = get<Point3f>(mesh, face);
    const auto e1 = p1 - p3;
    const auto e2 = p2 - p3;
    return cross(e1, e2).norm() / 2;
}

std::tuple<Point3f, Vector3f, f32> Triangle::sample(const Point2f& random) const
{
    const auto r0 = std::sqrt(random[0]);
    const auto alpha = 1 - r0;
    const auto beta = r0 * random[1];
    const auto gamma = 1 - alpha - beta;

    Point3f random_p;
    const auto& [p1, p2, p3] = get<Point3f>(mesh, face);
    random_p = interpolate(alpha, beta, gamma, p1, p2, p3);

    Vector3f normal;
    if(mesh->normals)
    {
        const auto& [n1, n2, n3] = get<Vector3f>(mesh, face);
        normal = interpolate(alpha, beta, gamma, n1, n2, n3);
    }
    return {random_p, normal, pdf()};
}

f32 Triangle::pdf() const
{
    return 1 / area();
}
