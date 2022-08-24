#include <raytracing/Core/Mesh/Mesh.hpp>

#include <string_view>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>
#include <ranges>

#include <util/util.hpp>

std::optional<Mesh> Mesh::read(const std::string_view& obj_path, const std::string_view& texture_path)
{
    Mesh mesh;
    if(!mesh.load_obj(obj_path) || mesh.empty())
    {
        std::cout << "Cannot open " << obj_path << '\n';
        return {};
    }

    if(!texture_path.empty())
    {
        if(!mesh.load_texture(texture_path))
        {
            std::cout << "Cannot open " << texture_path << '\n';
            return {};
        }
    }
    return std::make_optional<Mesh>(std::move(mesh));
}

bool Mesh::load_obj(const std::string_view& filename)
{
    if(!std::filesystem::exists(filename))
        return false;

    parse(filename);
    return true;
}

bool Mesh::load_texture(const std::string_view& filename)
{
    texture = Image::read(filename);
    return texture.has_value();
}

bool Mesh::empty() const
{
    return vertices.empty() || faces.empty();
}

void Mesh::info() const
{
    std::cout
        << "vertex: " << vertices.size() << '\n'
        << "face: "   << faces.size() << '\n'
        << std::boolalpha
        << "normal: " << !normals.empty() << '\n'
        << "uv: "     << !uvs.empty() << '\n';
}

static usize my_atoi(const std::string_view& str)
{
    usize number = 0;
    for(const auto& c : str)
    {
        if(c >= '0' && c <= '9')
        {
            number = number * 10 + (c - '0');
        }
    }
    return number;
}

template <template <arithmetic> typename T, arithmetic U>
T<U> string_to(const std::string_view& line)
{
    T<U> ret;
    usize i = 0;
    for(auto number : split(line, ' '))
    {
        if constexpr(std::is_floating_point_v<U>)
            ret[i] = static_cast<U>(std::atof(number.data()));
        else if constexpr(std::is_integral_v<U>)
            ret[i] = my_atoi(number.data());

        i++;
        if(i >= sizeof(T<U>) / sizeof(U))
            break;
    }
    return ret;
}

void Mesh::parse(const std::string_view& filename)
{
    std::ifstream in(filename.data(), std::ifstream::in);

    const auto size = std::filesystem::file_size(filename.data());
    auto buffer = std::make_unique<char[]>(size + 1);
    buffer[size] = '\0';
    in.read(buffer.get(), size);

    for(auto line : split(std::string_view{buffer.get(), size}, '\n'))
    {
        if(line[0] == 'v')
        {
            if(line[1] == ' ')
            {
                vertices.emplace_back(string_to<Point3, f32>(line.substr(2)));
            }
            else if(line[1] == 'n' && line[2] == ' ')
            {
                normals.emplace_back(string_to<Vector3, f32>(line.substr(3)));
            }
            else if(line[1] == 't' && line[2] == ' ')
            {
                uvs.emplace_back(string_to<Point2, f32>(line.substr(3)));
            }
        }
        else if(line[0] == 'f' && line[1] == ' ')
        {
            Face f;
            usize i = 0;
            for(auto face_line : split(line.substr(2), ' '))
            {
                usize j = 0;
                for(auto number : split(face_line, '/'))
                {
                    f.vertices[i][j] = my_atoi(number) - 1;
                    j++;
                }
                i++;
            }
            faces.emplace_back(f);
        }
    }
    in.close();
}
