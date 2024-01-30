#include "cirrus/ast/expr/parenthesis.hpp"

namespace cirrus::ast {

const ExpressionVtbl ParenthesisExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    ParenthesisExpressionData* _data =
                        static_cast<ParenthesisExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::ParenthesisExpression,
    },
};

ParenthesisExpression ParenthesisExpression::alloc(const Expression expr) noexcept {
    ParenthesisExpressionData* data = new ParenthesisExpressionData{};
    data->expr                      = expr;
    return ParenthesisExpression::from_raw(&ParenthesisExpression::_vtbl, data);
}

}  // namespace cirrus::ast
