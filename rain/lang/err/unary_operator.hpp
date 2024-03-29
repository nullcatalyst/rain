#pragma once

#include <string>

#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class UnaryOperatorError : public util::Error {
    lex::Location _expression_location;
    lex::Location _op_location;
    std::string   _msg;

  public:
    UnaryOperatorError(lex::Location expression_location, lex::Location op_location,
                       std::string msg)
        : _expression_location(expression_location),
          _op_location(op_location),
          _msg(std::move(msg)) {}
    ~UnaryOperatorError() override = default;

    [[nodiscard]] std::string message() const noexcept override;
};

}  // namespace rain::lang::err
