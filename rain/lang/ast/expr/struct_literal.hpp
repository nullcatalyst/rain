#pragma once

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

struct StructLiteralField {
    std::string_view            name;
    std::unique_ptr<Expression> value;

    // The index of the field in the struct type.
    int index = -1;
};

class StructLiteralExpression : public Expression {
    util::MaybeOwnedPtr<Type>       _type;
    std::vector<StructLiteralField> _fields;

    lex::Location _location;

  public:
    StructLiteralExpression(util::MaybeOwnedPtr<Type> type, std::vector<StructLiteralField> fields,
                            lex::Location location)
        : _type(std::move(type)), _fields(std::move(fields)), _location(location) {}
    ~StructLiteralExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::StructLiteral;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _type.get();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] bool                    compile_time_capable() const noexcept override {
        for (const auto& field : _fields) {
            if (!field.value->compile_time_capable()) {
                return false;
            }
        }

        return true;
    }

    // StructLiteralExpression
    [[nodiscard]] constexpr const std::vector<StructLiteralField>& fields() const {
        return _fields;
    }
    [[nodiscard]] constexpr std::vector<StructLiteralField>& fields() { return _fields; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
