#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "absl/base/nullability.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class ExternExpression : public Expression {
    llvm::SmallVector<std::string, 3>              _keys;
    std::unique_ptr<FunctionDeclarationExpression> _declaration;

    bool _compile_time_capable = false;

    lex::Location _location;

  public:
    ExternExpression(llvm::SmallVector<std::string, 3>              keys,
                     std::unique_ptr<FunctionDeclarationExpression> declaration,
                     lex::Location                                  location)
        : _keys(std::move(keys)), _declaration(std::move(declaration)), _location(location) {}
    ~ExternExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Extern;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _declaration->type();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] constexpr bool is_compile_time_capable() const noexcept override {
        return _compile_time_capable;
    }

    // ExternExpression
    [[nodiscard]] constexpr const llvm::SmallVector<std::string, 3>& keys() const { return _keys; }
    [[nodiscard]] /*constexpr*/ const FunctionDeclarationExpression& declaration() const {
        return *_declaration;
    }
    [[nodiscard]] /*constexpr*/ FunctionDeclarationExpression& declaration() {
        return *_declaration;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
