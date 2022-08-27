#pragma once

#include <vector>
#include <array>

#include <math/Vector3.hpp>
#include <math/Point3.hpp>
#include <math/Point2.hpp>

#include <util/Image.hpp>

struct Face
{
    struct vertex
    {
        usize point, uv, normal;

        usize& operator [] (usize i) { return (&point)[i]; }
    };

    std::array<vertex, 3> vertices;
};

struct Mesh
{
    std::vector<Point3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Point2f> uvs;
    std::vector<Face> faces;

    std::optional<Image> texture;

private:
    void parse(const char* path);

public:
    static std::optional<Mesh> read(const char* obj_path, const char* texture_path = "");

    bool load_obj(const char* path);

    bool load_texture(const char* path);

    bool empty() const;

    void info() const;
};
