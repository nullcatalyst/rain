#include "cirrus/ast/expr/call.hpp"

namespace cirrus::ast {

const ExpressionVtbl CallExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Call),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const CallExpressionData* _data = static_cast<CallExpressionData*>(data);
            if (!_data->callee.compile_time_able()) {
                return false;
            }
            for (const Expression& arg : _data->arguments) {
                if (!arg.compile_time_able()) {
                    return false;
                }
            }
            return true;
        },
};

CallExpression CallExpression::alloc(Expression              callee,
                                     std::vector<Expression> arguments) noexcept {
    CallExpressionData* data = new CallExpressionData{};
    data->callee             = std::move(callee);
    data->arguments          = std::move(arguments);
    return CallExpression::from_raw(&CallExpression::_vtbl, data);
}

}  // namespace cirrus::ast
