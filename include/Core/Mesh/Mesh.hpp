#pragma once

#include <vector>
#include <array>

#include <Hinae/Vector3.hpp>
#include <Hinae/Point3.hpp>
#include <Hinae/Point2.hpp>

using namespace Hinae;

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
    void parse(const std::string_view& filename);

public:
    static std::optional<Mesh> read(const std::string_view& obj_path, const std::string_view& texture_path = "");

    bool load_obj(const std::string_view& filename);

    bool load_texture(const std::string_view& filename);

    bool empty() const;

    void info() const;
};
