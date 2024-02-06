#include "cirrus/ast/expr/integer.hpp"

namespace cirrus::ast {

const ExpressionVtbl IntegerExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Integer),
    .compile_time_able = [](const ExpressionVtbl* const vtbl,
                            ExpressionData* const       data) noexcept { return true; },
};

IntegerExpression IntegerExpression::alloc(const uint64_t value) noexcept {
    IntegerExpressionData* data = new IntegerExpressionData{};
    data->value                 = value;
    return IntegerExpression::from_raw(&IntegerExpression::_vtbl, data);
}

}  // namespace cirrus::ast
