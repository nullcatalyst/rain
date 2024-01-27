#include "cirrus/ast/expr/member.hpp"

namespace cirrus::ast {

const ExpressionVtbl MemberExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    MemberExpressionData* _data = static_cast<MemberExpressionData*>(data);
                    delete _data;
                },
        },
        .kind = NodeKind::MemberExpression,
    },
};

MemberExpression MemberExpression::alloc(Expression expr, std::string_view member) noexcept {
    MemberExpressionData* data = new MemberExpressionData{};
    data->expr                 = std::move(expr);
    data->member               = member;
    return MemberExpression::from_raw(&MemberExpression::_vtbl, data);
}

}  // namespace cirrus::ast
