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

fn read_file(std::string_view filename) -> std::tuple<std::unique_ptr<char[]>, usize>;

enum class parse_error
{
    Cannot_open_file,
    Cannot_parse_string,
    Missing_bracket,
    Missing_comma,
};

std::ostream& operator << (std::ostream& os, parse_error error);

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

    fn parse_object() -> Result<std::vector<Object>, parse_error>;

    fn parse_property() -> Result<std::vector<Object>, parse_error>;

public:
    static fn parse(std::string_view filename) -> Result<std::vector<Object>, parse_error>;
};
