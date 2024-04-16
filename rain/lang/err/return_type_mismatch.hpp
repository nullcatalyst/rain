#pragma once

#include <string>

#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class ReturnTypeMismatchError : public util::Error {
    lex::Location _return_location;
    std::string   _return_type_name;
    lex::Location _declaration_location;
    std::string   _delcaration_type_name;

  public:
    ReturnTypeMismatchError(lex::Location return_location, std::string return_type_name,
                            lex::Location declaration_location, std::string declaration_type_name)
        : _return_location(return_location),
          _return_type_name(std::move(return_type_name)),
          _declaration_location(declaration_location),
          _delcaration_type_name(std::move(declaration_type_name)) {}
    ~ReturnTypeMismatchError() override = default;

    [[nodiscard]] std::string message() const noexcept override;
};

}  // namespace rain::lang::err
