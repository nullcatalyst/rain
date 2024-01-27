#pragma once

#include <string>

#include "cirrus/util/result.hpp"

namespace cirrus::err {

class SimpleError : public util::Error {
    std::string _msg;

  public:
    SimpleError(std::string msg) : _msg(std::move(msg)) {}
    ~SimpleError() override = default;

    [[nodiscard]] std::string message() const noexcept override { return _msg; }
};

}  // namespace cirrus::err
