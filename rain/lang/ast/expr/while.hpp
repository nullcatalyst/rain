#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class WhileExpression : public Expression {
    std::unique_ptr<Expression>                     _condition;
    std::unique_ptr<BlockExpression>                _loop;
    std::optional<std::unique_ptr<BlockExpression>> _else;

    absl::Nullable<Type*> _type = nullptr;

    lex::Location _while_location;
    lex::Location _else_location;

  public:
    WhileExpression(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> loop,
                    lex::Location while_location)
        : _condition(std::move(condition)),
          _loop(std::move(loop)),
          _while_location(while_location) {}
    WhileExpression(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> loop,
                    std::unique_ptr<BlockExpression> else_, lex::Location while_location,
                    lex::Location else_location)
        : _condition(std::move(condition)),
          _loop(std::move(loop)),
          _else(std::move(else_)),
          _while_location(while_location),
          _else_location(else_location) {}
    ~WhileExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::While;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return has_else() ? _while_location.merge(_else.value()->location())
                                  : _while_location.merge(_loop->location());
    }
    [[nodiscard]] bool compile_time_capable() const noexcept override;

    // WhileExpression
    [[nodiscard]] /*constexpr*/ const Expression& condition() const noexcept { return *_condition; }
    [[nodiscard]] /*constexpr*/ Expression&       condition() noexcept { return *_condition; }

    [[nodiscard]] /*constexpr*/ const BlockExpression& loop() const noexcept { return *_loop; }
    [[nodiscard]] /*constexpr*/ BlockExpression&       loop() noexcept { return *_loop; }

    [[nodiscard]] constexpr bool has_else() const noexcept { return _else.has_value(); }
    [[nodiscard]] /*constexpr*/ const BlockExpression& else_() const noexcept {
        return *_else.value();
    }
    [[nodiscard]] /*constexpr*/ BlockExpression& else_() noexcept { return *_else.value(); }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
