#pragma once

#include <ranges>

#include "basic_type.hpp"

#if defined(__clang__)
template <std::integral T>
struct range
{
private:
    T start, stop, step;

private:
    struct range_iterator
    {
    private:
        T value, step;

    public:
        constexpr range_iterator(T value, T step) : value(value), step(step) {}
        T& operator * () { return value; }
        range_iterator& operator ++ () { value += step; return *this; }
        bool operator != (const range_iterator& other) { return value < other.value; }
    };

public:
    constexpr range(T start, T stop, T step = 1) : start(start), stop(stop), step(step) {}
    constexpr range(T stop) : range(0, stop) {}
    constexpr range_iterator begin() { return range_iterator(start, step); }
    constexpr range_iterator end() { return range_iterator(stop, step); }

    range() = delete;
};

template<typename T>
struct Enumerate
{
private:
    T it;
    usize index;
    const usize step;
    decltype(std::begin(it)) begin_;
    const decltype(std::end(it)) end_;

public:
    Enumerate(T it, usize step) : it(it), index(0), step(step),
        begin_(std::begin(it)), end_(std::end(it)) {}

    Enumerate& begin() { return *this; }
    Enumerate& end()   { return *this; }

    auto operator != (const Enumerate&) const -> bool
    {
        return begin_ != end_;
    }

    auto operator ++ ()
    {
        begin_++;
        index  += step;
    }

    auto operator * () const -> std::pair<usize, decltype(*begin_)>
    {
        return {index, *begin_};
    }
};

template<std::ranges::range T>
auto enumerate(T& it, usize step = 1) -> Enumerate<T>
{
    return {std::forward<T>(it), step};
}

template <typename T>
struct split_iterator
{
private:
    const std::string_view& sv;
    const T& delim;
    usize start_, end_;

public:
    split_iterator(const std::string_view& sv, const T& delim)
        : sv(sv), delim(delim)
        , start_(find_first_not(0))
        , end_(sv.find(delim, start_)) {}

    split_iterator& begin() { return *this; }
    split_iterator& end()   { return *this; }

    auto find_first_not(usize pos = 0) const -> usize
    {
        for (usize i = pos; i < sv.size(); i += 1)
        {
            if (sv[i] != delim) return i;
        }
        return std::string_view::npos;
    };   

    auto operator != (const split_iterator&) const -> bool
    {
        return end_ != std::string_view::npos
            && start_ != std::string_view::npos;
    }

    auto operator ++ ()
    {
        start_ = find_first_not(end_);
        end_ = sv.find(delim, start_);
        if(end_ == std::string_view::npos)
        {
            end_ = sv.size();
        }
    }

    auto operator * () const -> std::string_view
    {
        return {&sv[start_], end_ - start_};
    }
};

template <typename T>
constexpr auto split(const std::string_view& sv, const T& delim)
{
    return split_iterator{sv, delim};
}
#else
template <std::integral T>
constexpr auto range(T end)
{
    return std::views::iota(static_cast<T>(0), end);
}

template <std::integral T>
constexpr auto range(T start, T end)
{
    return std::views::iota(start, end);
}

template <typename T>
struct enumerated_element
{
    usize index;
    T& element;
};

template <std::ranges::range T>
constexpr auto enumerate(T& range, usize step = 1)
{
    return range | std::views::transform(
        [i = usize{}, step](auto& element) mutable
        {
            i += step;
            return enumerated_element{i - step, element};
        }
    );
}

template <typename T>
constexpr auto split(const std::string_view& sv, const T& delim)
{
    return sv
        | std::ranges::views::split(delim)
        | std::ranges::views::transform([](auto&& rng)
    {
        return std::string_view(std::addressof(*rng.begin()), std::ranges::distance(rng));
    });
}

#endif // defined(__clang__)
