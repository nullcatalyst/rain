#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "tl/expected.hpp"

namespace rain::util {

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

}  // namespace rain::util
