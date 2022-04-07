#include <util/util.hpp>
#include <Renderer.hpp>

int main(int argc, char const *argv[])
{
    Result<Renderer, std::string_view> result = Renderer::init(argc, argv);
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
