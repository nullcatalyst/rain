#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "tl/expected.hpp"

namespace cirrus::util {
/*
#define OK(ResultType, ...) util::Result<ResultType>::ok(__VA_ARGS__)

#define ERR(ResultType, ErrorType, ...) util::Result<ResultType>::error(__VA_ARGS__)

#define ERR_PTR(ResultType, ErrorType, ...) \
    util::Result<ResultType>::error(std::make_unique<ErrorType>(__VA_ARGS__))

#define FORWARD_ERROR(result) \
    if (result.is_error()) {  \
        return result;        \
    }

#define FORWARD_ERROR_WITH_TYPE(ResultType, result)                               \
    if (result.is_error()) {                                                      \
        static_assert(!std::is_const_v<decltype(result)>,                         \
                      "cannot forward an error with a const result type");        \
        return util::Result<ResultType>::error(std::move(result.unwrap_error())); \
    }

// Assert that the result is not an error, and print the error message if it is, before failing
#define ASSERT_NOT_ERROR(result)                                    \
    if (result.is_error()) {                                        \
        std::cerr << result.unwrap_error()->message() << std::endl; \
        ASSERT_EQ(result.is_ok(), true);                            \
    }
*/

#define ERR(ErrorType, ...) tl::unexpected(ErrorType{__VA_ARGS__})

#define ERR_PTR(ErrorType, ...) tl::unexpected(std::make_unique<ErrorType>(__VA_ARGS__))

#define FORWARD_ERROR(result)                                              \
    do {                                                                   \
        static_assert(!std::is_const_v<decltype(result)>,                  \
                      "cannot forward an error with a const result type"); \
        if (!result.has_value()) {                                         \
            return tl::unexpected(std::move(result).error());              \
        }                                                                  \
    } while (0)

// Assert that the result is not an error, and print the error message if it is, before failing
#define ASSERT_NOT_ERROR(result)                                    \
    if (!result.has_value()) {                                      \
        std::cerr << result.unwrap_error()->message() << std::endl; \
        ASSERT_EQ(result.has_value(), true);                        \
    }

class Error {
  public:
    virtual ~Error() = default;

    [[nodiscard]] virtual std::string message() const noexcept = 0;
};

template <typename T, typename E = std::unique_ptr<Error>>
using Result = tl::expected<T, E>;

/*
template <typename T, typename E = std::unique_ptr<Error>>
class Result {
    using Raw = std::variant<T, E>;

    Raw _raw;

    constexpr Result(Raw raw) noexcept : _raw(std::move(raw)) {}

  public:
    constexpr Result() noexcept : _raw(std::in_place_index<0>) {
        static_assert(std::is_default_constructible_v<T>,
                      "the result type is not default constructible");
    }

    constexpr Result(const Result& other) noexcept = default;
    constexpr Result(Result&& other) noexcept      = default;

    constexpr Result& operator=(const Result& other) noexcept = default;
    constexpr Result& operator=(Result&& other) noexcept      = default;

    template <typename T2>
    [[nodiscard]] constexpr Result(const Result<T2>& other) noexcept {
        static_assert(std::is_constructible_v<T, T2>, "cannot cast to an unrelated result type");
        static_assert(std::is_copy_constructible_v<E>, "the result error type is not copyable");

        if (other.is_ok()) {
            _raw = Raw(std::in_place_index<0>, other.unwrap());
        } else {
            _raw = Raw(std::in_place_index<1>, other.unwrap_error());
        }
    }

    template <typename T2>
    [[nodiscard]] constexpr Result(Result<T2>&& other) noexcept {
        static_assert(std::is_constructible_v<T, T2>, "cannot cast to an unrelated result type");
        static_assert(std::is_move_constructible_v<E>, "the result error type is not moveable");

        if (other.is_ok()) {
            _raw = Raw(std::in_place_index<0>, std::move(other.unwrap()));
        } else {
            _raw = Raw(std::in_place_index<1>, std::move(other.unwrap_error()));
        }
    }

    [[nodiscard]] static constexpr Result ok(const T& value) {
        static_assert(std::is_copy_constructible_v<T>, "the result type is not copyable");

        return Result(Raw{
            std::in_place_index<0>,
            value,
        });
    }

    [[nodiscard]] static constexpr Result ok(T&& value) {
        static_assert(std::is_move_constructible_v<T>, "the result type is not moveable");

        return Result(Raw{
            std::in_place_index<0>,
            std::move(value),
        });
    }

    [[nodiscard]] static constexpr Result error(const E& error) {
        static_assert(std::is_copy_constructible_v<E>, "the error type is not copyable");

        return Result(Raw{
            std::in_place_index<1>,
            error,
        });
    }

    [[nodiscard]] static constexpr Result error(E&& error) {
        static_assert(std::is_move_constructible_v<E>, "the error type is not moveable");

        return Result(Raw{
            std::in_place_index<1>,
            std::move(error),
        });
    }

    [[nodiscard]] constexpr bool is_ok() const noexcept { return _raw.index() == 0; }
    [[nodiscard]] constexpr bool is_error() const noexcept { return _raw.index() == 1; }

    [[nodiscard]] constexpr T& unwrap() noexcept { return std::get<0>(_raw); }
    [[nodiscard]] constexpr E& unwrap_error() noexcept { return std::get<1>(_raw); }

    [[nodiscard]] constexpr const T& unwrap() const noexcept { return std::get<0>(_raw); }
    [[nodiscard]] constexpr const E& unwrap_error() const noexcept { return std::get<1>(_raw); }
};

template <typename E>
class Result<void, E> {
    using Raw = std::optional<E>;

    Raw _raw;

    constexpr Result(Raw raw) noexcept : _raw(std::move(raw)) {}

  public:
    constexpr Result() noexcept = default;

    constexpr Result(const Result& other) noexcept = default;
    constexpr Result(Result&& other) noexcept      = default;

    constexpr Result& operator=(const Result& other) noexcept = default;
    constexpr Result& operator=(Result&& other) noexcept      = default;

    template <typename T2>
    [[nodiscard]] constexpr Result(const Result<T2>& other) noexcept {
        static_assert(std::is_copy_constructible_v<E>, "the result error type is not copyable");

        if (other.is_error()) {
            _raw = other.unwrap_error();
        }
    }

    template <typename T2>
    [[nodiscard]] constexpr Result(Result<T2>&& other) noexcept {
        static_assert(std::is_move_constructible_v<E>, "the result error type is not moveable");

        if (other.is_error()) {
            _raw = std::move(other.unwrap_error());
        }
    }

    [[nodiscard]] static constexpr Result ok() { return Result(std::nullopt); }
    [[nodiscard]] static constexpr Result error(E error) { return Result(std::move(error)); }

    [[nodiscard]] constexpr bool is_ok() const noexcept { return !_raw.has_value(); }
    [[nodiscard]] constexpr bool is_error() const noexcept { return _raw.has_value(); }

    [[nodiscard]] constexpr E&       unwrap_error() noexcept { return _raw.value(); }
    [[nodiscard]] constexpr const E& unwrap_error() const noexcept { return _raw.value(); }
};
*/
}  // namespace cirrus::util
