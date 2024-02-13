#include "cirrus/ast/expr/function.hpp"

namespace cirrus::ast {

const ExpressionVtbl FunctionExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Function),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const FunctionExpressionData* _data = static_cast<FunctionExpressionData*>(data);
            for (const Expression& expr : _data->expressions) {
                if (!expr.compile_time_able()) {
                    return false;
                }
            }
            return true;
        },
};

FunctionExpression FunctionExpression::alloc(std::optional<util::String>       name,
                                             std::optional<Type>               return_type,
                                             std::vector<FunctionArgumentData> arguments,
                                             std::vector<Expression> expressions) noexcept {
    FunctionExpressionData* data = new FunctionExpressionData{};
    data->name                   = std::move(name);
    data->return_type            = std::move(return_type);
    data->arguments              = std::move(arguments);
    data->expressions            = std::move(expressions);
    return FunctionExpression::from_raw(&FunctionExpression::_vtbl, data);
}

}  // namespace cirrus::ast
