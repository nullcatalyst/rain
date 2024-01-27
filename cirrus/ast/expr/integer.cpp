#include "cirrus/ast/expr/integer.hpp"

namespace cirrus::ast {

const ExpressionVtbl IntegerExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    IntegerExpressionData* _data = static_cast<IntegerExpressionData*>(data);
                    delete _data;
                },
        },
        .kind = NodeKind::IntegerExpression,
    },
};

IntegerExpression IntegerExpression::alloc(const uint64_t value) noexcept {
    IntegerExpressionData* data = new IntegerExpressionData{};
    data->value                 = value;
    return IntegerExpression::from_raw(&IntegerExpression::_vtbl, data);
}

}  // namespace cirrus::ast
