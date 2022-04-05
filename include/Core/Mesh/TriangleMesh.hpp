#pragma once

#include <memory>

#include <Hinae/Matrix4.hpp>

using namespace Hinae;

#include "Mesh.hpp"

struct TriangleMesh
{
    const usize total_vertices, total_faces;
    std::unique_ptr<Face[]> faces;
    std::unique_ptr<Point3f[]> vertices;
    std::unique_ptr<Vector3f[]> normals;
    std::unique_ptr<Point2f[]> uvs;

    TriangleMesh(const Matrix4f& local_to_world
        , const std::vector<Face>& load_face
        , const std::vector<Point3f>& load_vertex
        , const std::vector<Vector3f>& load_normal
        , const std::vector<Point2f>& load_uv);
};
