#include "cirrus/ast/expr/export.hpp"

namespace cirrus::ast {

const ExpressionVtbl ExportExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    ExportExpressionData* _data = static_cast<ExportExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::ExportExpression,
    },
};

ExportExpression ExportExpression::alloc(Expression expression) noexcept {
    ExportExpressionData* data = new ExportExpressionData{};
    data->expression           = std::move(expression);
    return ExportExpression::from_raw(&ExportExpression::_vtbl, data);
}

}  // namespace cirrus::ast
