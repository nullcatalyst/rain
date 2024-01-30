#include "cirrus/ast/expr/identifier.hpp"

namespace cirrus::ast {

const ExpressionVtbl IdentifierExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    IdentifierExpressionData* _data = static_cast<IdentifierExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::IdentifierExpression,
    },
};

IdentifierExpression IdentifierExpression::alloc(const std::string_view name) noexcept {
    IdentifierExpressionData* data = new IdentifierExpressionData{};
    data->name                     = name;
    return IdentifierExpression::from_raw(&IdentifierExpression::_vtbl, data);
}

}  // namespace cirrus::ast
