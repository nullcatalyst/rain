#pragma once

#include <string>

#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class BinaryOperatorError : public util::Error {
    lex::Location _lhs_location;
    lex::Location _rhs_location;
    lex::Location _op_location;
    std::string   _msg;

  public:
    BinaryOperatorError(lex::Location lhs_location, lex::Location rhs_location,
                        lex::Location op_location, std::string msg)
        : _lhs_location(lhs_location),
          _rhs_location(rhs_location),
          _op_location(op_location),
          _msg(std::move(msg)) {}
    ~BinaryOperatorError() override = default;

    [[nodiscard]] std::string message() const noexcept override;
};

}  // namespace rain::lang::err
