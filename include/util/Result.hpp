#pragma once

#include <variant>

template <typename T>
struct Ok
{
public:
    T value;

public:
    explicit constexpr Ok(T value) : value(std::move(value)) {}

    constexpr T&& take_value() { return std::move(value); }
};

template <typename T>
struct Err
{
public:
    T value;

public:
    explicit constexpr Err(T value) : value(std::move(value)) {}

    constexpr T&& take_value() { return std::move(value); }
};

template <typename OkT, typename ErrT>
class Result
{
public:
    std::variant<Ok<OkT>, Err<ErrT>> variant;

public:

    constexpr Result(Ok<OkT> value) : variant(std::move(value)) {}
    constexpr Result(Err<ErrT> value) : variant(std::move(value)) {}

    constexpr bool is_ok() const { return std::holds_alternative<Ok<OkT>>(variant); }
    constexpr bool is_err() const { return std::holds_alternative<Err<ErrT>>(variant); }

    constexpr OkT ok_value() const { return std::get<Ok<OkT>>(variant).value; }
    constexpr ErrT err_value() const { return std::get<Err<ErrT>>(variant).value; }

    constexpr OkT&& take_ok_value() { return std::get<Ok<OkT>>(variant).take_value(); }
    constexpr ErrT&& take_err_value() { return std::get<Err<ErrT>>(variant).take_value(); }
};
