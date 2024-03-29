#pragma once

#include <string>

#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class SyntaxError : public util::Error {
    lex::Location _location;
    std::string   _msg;

  public:
    SyntaxError(lex::Location location, std::string msg)
        : _location(location), _msg(std::move(msg)) {}
    ~SyntaxError() override = default;

    [[nodiscard]] std::string message() const noexcept override;
};

}  // namespace rain::lang::err
