#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"

namespace rain::lang::ast {

class MethodExpression : public FunctionExpression {
    /** The type of the callee. */
    absl::Nonnull<Type*> _callee_type;

  public:
    MethodExpression(std::optional<util::String> name, ast::TypePtr method_owner,
                     std::optional<TypePtr>            return_type,
                     std::vector<FunctionArgumentData> arguments, BlockPtr block)
        : _name{std::move(name)},
          _method_owner{std::move(method_owner)},
          _return_type{std::move(return_type)},
          _arguments{std::move(arguments)},
          _block{std::move(block)} {}
    FunctionExpression(std::optional<util::String> name, ast::TypePtr method_owner,
                       std::optional<TypePtr>            return_type,
                       std::vector<FunctionArgumentData> arguments, BlockPtr block,
                       lang::Location fn_location)
        : Expression(fn_location.merge(block->location())),
          _name{std::move(name)},
          _method_owner{std::move(method_owner)},
          _return_type{std::move(return_type)},
          _arguments{std::move(arguments)},
          _block{std::move(block)},
          _fn_location(fn_location) {}

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
