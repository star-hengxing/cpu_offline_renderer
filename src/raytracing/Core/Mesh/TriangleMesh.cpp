#include <raytracing/Core/Mesh/TriangleMesh.hpp>

#include <cstring>

#include <math/Transform.hpp>

#include <util/util.hpp>

TriangleMesh::TriangleMesh(const Matrix4f& local_to_world
    , const std::vector<Face>& load_face
    , const std::vector<Point3f>& load_vertex
    , const std::vector<Vector3f>& load_normal
    , const std::vector<Point2f>& load_uv)
    : total_vertices(load_vertex.size()), total_faces(load_face.size() / 3)
{
    if(load_face.empty() || load_vertex.empty())
        return;

    faces = std::make_unique<Face[]>(load_face.size());
    std::memcpy(faces.get(), load_face.data(), sizeof(Face) * load_face.size());

    vertices = std::make_unique<Point3f[]>(load_vertex.size());
    for(auto [i, v] : enumerate(load_vertex))
    {
        vertices[i] = local_to_world * v;
    }

    if(!load_normal.empty())
    {
        normals = std::make_unique<Vector3f[]>(load_normal.size());
        for(auto [i, v] : enumerate(load_normal))
        {
            normals[i] = local_to_world * v;
        }
    }

    if(!load_uv.empty())
    {
        uvs = std::make_unique<Point2f[]>(load_uv.size());
        std::memcpy(uvs.get(), load_uv.data(), sizeof(Point2f) * load_uv.size());
    }
}
