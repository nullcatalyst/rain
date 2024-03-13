#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

struct FunctionArgument {
    std::string_view     name;
    absl::Nonnull<Type*> type;
};

class FunctionExpression : public Expression {
    /** The name of the function, if it has one. */
    std::optional<std::string_view> _name;

    /** The set of arguments passed to the function. */
    std::vector<FunctionArgument> _arguments;

    /** The return type of the function, or null if the function does not return a value. */
    absl::Nullable<Type*> _return_type;

    /** The function body. */
    std::unique_ptr<BlockExpression> _block;

    /**
     * This is the variable representing this function.
     * This value is null if the function has no name.
     */
    absl::Nullable<FunctionVariable*> _function_variable;

  public:
    FunctionExpression(std::optional<std::string_view> name, absl::Nullable<Type*> return_type,
                       std::vector<FunctionArgument> arguments)
        : _name{std::move(name)},
          _method_owner{std::move(method_owner)},
          _return_type{std::move(return_type)},
          _arguments{std::move(arguments)},
          _block{std::move(block)} {}

    [[nodiscard]] static std::unique_ptr<FunctionExpression> alloc(
        std::optional<util::String> name, ast::TypePtr method_owner,
        std::optional<TypePtr> return_type, std::vector<FunctionArgumentData> arguments,
        BlockPtr block) {
        return std::make_unique<FunctionExpression>(std::move(name), std::move(method_owner),
                                                    std::move(return_type), std::move(arguments),
                                                    std::move(block));
    }
    [[nodiscard]] static std::unique_ptr<FunctionExpression> alloc(
        std::optional<util::String> name, ast::TypePtr method_owner,
        std::optional<TypePtr> return_type, std::vector<FunctionArgumentData> arguments,
        BlockPtr block, lang::Location fn_location) {
        return std::make_unique<FunctionExpression>(std::move(name), std::move(method_owner),
                                                    std::move(return_type), std::move(arguments),
                                                    std::move(block), fn_location);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::FunctionExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    [[nodiscard]] constexpr bool is_named() const noexcept { return _name.has_value(); }
    [[nodiscard]] util::String   name_or_empty() const noexcept {
        return _name.value_or(util::String{});
    }

    [[nodiscard]] /*constexpr*/ bool has_method_owner() const noexcept {
        return _method_owner != nullptr;
    }
    [[nodiscard]] const TypePtr& method_owner() const noexcept { return _method_owner; }

    [[nodiscard]] constexpr bool has_return_type() const noexcept {
        return _return_type.has_value();
    }
    [[nodiscard]] constexpr const std::optional<TypePtr>& return_type() const noexcept {
        return _return_type;
    }

    [[nodiscard]] constexpr bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const std::vector<FunctionArgumentData>& arguments() const& noexcept {
        return _arguments;
    }
    [[nodiscard]] constexpr std::vector<FunctionArgumentData>&& arguments() && noexcept {
        return std::move(_arguments);
    }

    [[nodiscard]] const BlockPtr& block() const& noexcept { return _block; }
    [[nodiscard]] BlockPtr&&      block() && noexcept { return std::move(_block); }
};

}  // namespace rain::lang::ast
