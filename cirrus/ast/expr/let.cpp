#include "cirrus/ast/expr/let.hpp"

namespace cirrus::ast {

const ExpressionVtbl LetExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Let),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const LetExpressionData* _data = static_cast<LetExpressionData*>(data);
            return _data->_value.compile_time_able();
        },
};

LetExpression LetExpression::alloc(util::Twine name, Expression value, bool mutable_) noexcept {
    LetExpressionData* data = new LetExpressionData{};
    data->_name             = std::move(name);
    data->_value            = std::move(value);
    data->_mutable          = mutable_;
    return LetExpression::from_raw(&LetExpression::_vtbl, data);
}

}  // namespace cirrus::ast
