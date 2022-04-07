#include <util/Parser.hpp>

fn read_file(const std::string_view& filename) -> std::tuple<std::unique_ptr<char[]>, usize>
{
    std::ifstream in(filename.data(), std::ifstream::in);
    if(!in) return {nullptr, 0};

    const auto size = std::filesystem::file_size(filename.data());
    auto buffer = std::make_unique<char[]>(size + 1);
    in.read(buffer.get(), size);
    buffer[size] = '\0';
    in.close();
    return {std::move(buffer), size};
}

Parser::Parser(i8* p) : current(p) {}

fn Parser::is_end() -> bool
{
    return get() == '\0';
}

fn Parser::get() -> i8
{
    return *current;
}

fn Parser::next() -> Parser&
{
    current += 1;
    return *this;
}

fn Parser::next_and_get() -> i8
{
    return next().get();
}

fn Parser::parse_string() -> std::string
{
    std::string key;
    key.reserve(31);
    i8 c = get();
    while(std::isalpha(c) || c == '_')
    {
        key.push_back(c);
        c = next_and_get();
    }
    return key;
}

using namespace std::literals;

fn Parser::parse_property() -> Result<std::vector<Object>, std::string_view>
{
    if(!match('(')) return Err("Missing bracket"sv);
    next();

    std::vector<Object> vector;
    i8 c = get();

    loop
    {
        Object number;
        number.name.reserve(31);
        while(std::isalnum(c) || c == '.' || c == '-')
        {
            number.name.push_back(c);
            c = next_and_get();
        }

        vector.emplace_back(std::move(number));
        if(c != ')')
        {
            if(c == ',')
                c = next().skip().get();
            else
                return Err("Missing comma"sv);
        }
        else
        {
            next();
            return Ok(std::move(vector));
        }
    }
}

fn Parser::parse_object() -> Result<std::vector<Object>, std::string_view>
{
    if(!skip().match('{')) return Err("Missing bracket"sv);
    next();

    std::vector<Object> ret;
    loop
    {
        if(skip().match('}'))
        {
            next();
            return Ok(std::move(ret));
        }
        std::string name = parse_string();
        if(name.empty())
        {
            return Err("Cannot parse string"sv);
        }

        skip();
        if(match('{'))
        {
            auto result = parse_object();
            if(result.is_err()) return Err(result.take_err_value());
            ret.emplace_back(Object{std::move(name), result.take_ok_value()});
        }
        else if(match('('))
        {
            auto result = parse_property();
            if(result.is_err()) return Err(result.take_err_value());
            ret.emplace_back(Object{std::move(name), result.take_ok_value()});
        }
        else
        {
            return Err("Missing bracket"sv);
        }
    };
}

fn Parser::parse(const std::string_view& filename) -> Result<std::vector<Object>, std::string_view>
{
    const auto [buffer, size] = read_file(filename);
    if(!buffer) return Err("Cannot open file"sv);

    Parser parse((i8*)buffer.get());
    std::vector<Object> ret;

    while(!parse.is_end())
    {
        std::string name = parse.skip().parse_string();
        if(name.empty())
        {
            if(parse.is_end()) break;

            return Err("Cannot parse string"sv);
        }

        auto result = parse.parse_object();
        if(result.is_err())
        {
            print("parse", name, "failed\n");
            return Err(result.take_err_value());
        }

        ret.emplace_back(Object{std::move(name), result.take_ok_value()});
    }
    return Ok(std::move(ret));
}
