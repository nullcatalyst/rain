#include "cirrus/ast/expr/return.hpp"

namespace cirrus::ast {

const ExpressionVtbl ReturnExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    ReturnExpressionData* _data = static_cast<ReturnExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::ReturnExpression,
    },
};

ReturnExpression ReturnExpression::alloc(const Expression expr) noexcept {
    ReturnExpressionData* data = new ReturnExpressionData{};
    data->expr                 = expr;
    return ReturnExpression::from_raw(&ReturnExpression::_vtbl, data);
}

}  // namespace cirrus::ast
