#pragma once

#include <vector>

#include "rain/ast/expr/block.hpp"
#include "rain/ast/expr/expression.hpp"
#include "rain/ast/type/type.hpp"
#include "rain/util/string.hpp"

namespace rain::ast {

struct FunctionArgumentData {
    util::String name;
    TypePtr      type;
};

class FunctionExpression : public Expression {
    std::optional<util::String>       _name;
    ast::TypePtr                      _method_owner;
    std::optional<TypePtr>            _return_type;
    std::vector<FunctionArgumentData> _arguments;
    std::unique_ptr<BlockExpression>  _block;

  public:
    FunctionExpression(std::optional<util::String> name, ast::TypePtr method_owner,
                       std::optional<TypePtr>            return_type,
                       std::vector<FunctionArgumentData> arguments, BlockPtr block)
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

    [[nodiscard]] constexpr bool has_method_owner() const noexcept {
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

}  // namespace rain::ast
