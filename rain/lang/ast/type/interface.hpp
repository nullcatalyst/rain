#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/serial/expression.hpp"

namespace rain::lang::ast {

class InterfaceType : public Type {
    /** The name of the interface. */
    std::string_view _name;

    /** The set of fields contained in the struct. */
    std::vector<std::unique_ptr<FunctionDeclarationExpression>> _methods;

    bool          _resolved = false;
    lex::Location _location;

  public:
    InterfaceType(std::string_view name) : _name(std::move(name)) {}

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Interface;
    }
    [[nodiscard]] std::string display_name() const noexcept override { return std::string(_name); }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // InterfaceType
    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }
    [[nodiscard]] constexpr const auto&      methods() const noexcept { return _methods; }

    void add_method(std::unique_ptr<FunctionDeclarationExpression> method) {
        _methods.push_back(std::move(method));
    }
    void set_location(lex::Location location) { _location = location; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
