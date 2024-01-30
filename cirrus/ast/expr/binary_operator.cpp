#include "cirrus/ast/expr/binary_operator.hpp"

namespace cirrus::ast {

const ExpressionVtbl BinaryOperatorExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    BinaryOperatorExpressionData* _data =
                        static_cast<BinaryOperatorExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::BinaryOperatorExpression,
    },
};

BinaryOperatorExpression BinaryOperatorExpression::alloc(Expression lhs, Expression rhs,
                                                         BinaryOperator op) noexcept {
    BinaryOperatorExpressionData* data = new BinaryOperatorExpressionData{};
    data->lhs                          = std::move(lhs);
    data->rhs                          = std::move(rhs);
    data->op                           = op;
    return BinaryOperatorExpression::from_raw(&BinaryOperatorExpression::_vtbl, data);
}

}  // namespace cirrus::ast
