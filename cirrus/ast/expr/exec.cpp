#include "cirrus/ast/expr/exec.hpp"

namespace cirrus::ast {

const ExpressionVtbl ExecExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    ExecExpressionData* _data = static_cast<ExecExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::ExecExpression,
    },
};

ExecExpression ExecExpression::alloc(Expression expression) noexcept {
    ExecExpressionData* data = new ExecExpressionData{};
    data->expression         = std::move(expression);
    return ExecExpression::from_raw(&ExecExpression::_vtbl, data);
}

}  // namespace cirrus::ast
