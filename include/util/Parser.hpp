#include <string_view>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include "Result.hpp"
#include "util.hpp"

#define loop while(true)
#define fn auto

using i8 = std::int8_t;

fn read_file(const std::string_view& filename) -> std::tuple<std::unique_ptr<char[]>, usize>;

struct Object
{
    std::string name;
    std::vector<Object> property;
};

struct Parser
{
private:
    i8* current;

private:
    Parser(i8* p);

    fn is_end() -> bool;

    fn get() -> i8;

    fn next() -> Parser&;
    
    fn next_and_get() -> i8;

    template <typename... Args>
    fn match(Args&&... args) -> bool
    {
        const i8 c = get();
        return ((c == args) || ...);
    }

    template <typename... Args>
    fn skip(Args&&... args) -> Parser&
    {
        if constexpr(sizeof...(args) == 0)
        {
            while(match('\n', ' ', '\t', '\r'))
                next();
        }
        else
        {
            while(match(args...))
                next();
        }
        return *this;
    }

    fn parse_string() -> std::string;

    fn parse_object() -> Result<std::vector<Object>, std::string_view>;

    fn parse_property() -> Result<std::vector<Object>, std::string_view>;

public:
    static fn parse(const std::string_view& filename) -> Result<std::vector<Object>, std::string_view>;
};
