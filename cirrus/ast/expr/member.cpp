#include "cirrus/ast/expr/member.hpp"

namespace cirrus::ast {

const ExpressionVtbl MemberExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Member),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const MemberExpressionData* _data = static_cast<MemberExpressionData*>(data);
            return _data->expr.compile_time_able();
        },
};

MemberExpression MemberExpression::alloc(Expression expr, util::Twine member) noexcept {
    MemberExpressionData* data = new MemberExpressionData{};
    data->expr                 = std::move(expr);
    data->member               = std::move(member);
    return MemberExpression::from_raw(&MemberExpression::_vtbl, data);
}

}  // namespace cirrus::ast
