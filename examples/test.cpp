#include <Core/Mesh/Mesh.hpp>
#include <util/util.hpp>

void obj_test(int argc, char const *argv[])
{
    if(argc == 2)
    {
        std::optional<Mesh> result = Timer::elapsed(Mesh::read, argv[1], "");
        if(result)
        {
            result->info();
        }
    }
}

int main(int argc, char const *argv[])
{
    obj_test(argc, argv);
}
