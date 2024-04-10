#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class InterfaceType : public Type {
    /** The name of the interface. */
    std::string_view _name;

    /** The set of fields contained in the struct. */
    std::vector<std::unique_ptr<FunctionDeclarationExpression>> _methods;

    lex::Location _location;

  public:
    InterfaceType(std::string_view                                            name,
                  std::vector<std::unique_ptr<FunctionDeclarationExpression>> methods,
                  lex::Location                                               location)
        : _name(std::move(name)), _methods(std::move(methods)), _location(location) {}

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Interface;
    }
    [[nodiscard]] std::string display_name() const noexcept override { return std::string(_name); }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // InterfaceType
    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }
    [[nodiscard]] constexpr const auto&      methods() const noexcept { return _methods; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
