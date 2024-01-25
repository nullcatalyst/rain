#include "cirrus/ast/expr/call.hpp"

namespace cirrus::ast {

const ExpressionVtbl CallExpression::_vtbl{
    /* util::RetainableVtbl */ {
        .retain  = util::_Retainable_vtbl.retain,
        .release = util::_Retainable_vtbl.release,
        .drop =
            [](const util::RetainableVtbl* const vtbl, util::RetainableData* const data) noexcept {
                CallExpressionData* _data = static_cast<CallExpressionData*>(data);
                delete _data;
            },
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
