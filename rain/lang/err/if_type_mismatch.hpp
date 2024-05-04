#pragma once

#include <string>

#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class IfTypeMismatchError : public util::Error {
    lex::Location _then_value_location;
    std::string   _then_value_type_name;
    lex::Location _else_value_location;
    std::string   _else_value_type_name;

  public:
    IfTypeMismatchError(lex::Location then_value_location, std::string then_value_type_name,
                        lex::Location else_value_location, std::string else_value_type_name)
        : _then_value_location(then_value_location),
          _then_value_type_name(std::move(then_value_type_name)),
          _else_value_location(else_value_location),
          _else_value_type_name(std::move(else_value_type_name)) {}
    ~IfTypeMismatchError() override = default;

    [[nodiscard]] std::string message() const noexcept override;
};

}  // namespace rain::lang::err
