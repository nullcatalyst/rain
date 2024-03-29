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
#define ASSERT_NOT_ERROR(result)                             \
    if (!result.has_value()) {                               \
        std::cerr << result.error()->message() << std::endl; \
        ASSERT_EQ(result.has_value(), true);                 \
    }

// Assert that the result is an error
#define ASSERT_ERROR(result) ASSERT_EQ(result.has_value(), false)

class Error {
  public:
    virtual ~Error() = default;

    [[nodiscard]] virtual std::string message() const noexcept = 0;
};

template <typename T, typename E = std::unique_ptr<Error>>
using Result = tl::expected<T, E>;

}  // namespace rain::util

#if defined(GTEST_TEST)

// Add a useful testing helper function, that prints out the error message if the result is an
// error. Hopefully this should make it easier to debug tests.
template <typename T>
testing::AssertionResult check_success(const rain::util::Result<T>& result) {
    if (result.has_value()) {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << result.error()->message();
}

#endif  // defined(GTEST_TEST)
