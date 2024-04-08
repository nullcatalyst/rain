#pragma once

#include <string>

#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class MultipleDefinitionError : public util::Error {
    lex::Location _first_location;
    lex::Location _second_location;
    std::string   _msg;

  public:
    MultipleDefinitionError(lex::Location first_location, lex::Location second_location,
                            std::string msg)
        : _first_location(first_location),
          _second_location(second_location),
          _msg(std::move(msg)) {}
    ~MultipleDefinitionError() override = default;

    [[nodiscard]] std::string message() const noexcept override;
};

}  // namespace rain::lang::err
