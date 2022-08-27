#include <string_view>
#include <filesystem>

#include <raytracing/Renderer/Renderer.hpp>
#include <util/io.hpp>

int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        println("invalid parameter");
        return -1;
    }

    const auto file = std::string_view{argv[1]};

    if (!file.ends_with(".json") || !std::filesystem::exists(file))
    {
        println("invalid scene file");
        return -1;
    }

    render(file.data());
}
