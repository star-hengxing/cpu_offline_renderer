#include <raytracing/Renderer.hpp>
#include <util/util.hpp>

int main(int argc, char const *argv[])
{
    Result<Renderer, std::string_view> result = Renderer::init(argc, argv, cornell_box);
    if(result.is_err())
    {
        println(result.err_value());
        return -1;
    }
    else
    {
        result.take_ok_value().render();
        return 0;
    }
}
