#pragma once

#include <string>

#include "rain/util/result.hpp"

namespace rain::lang::err {

class SimpleError : public util::Error {
    std::string _message;

  public:
    SimpleError(std::string message) : _message(std::move(message)) {}
    ~SimpleError() override = default;

    [[nodiscard]] std::string message() const noexcept override { return _message; }
};

}  // namespace rain::lang::err
