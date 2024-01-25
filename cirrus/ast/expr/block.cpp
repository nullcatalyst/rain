#include "cirrus/ast/expr/block.hpp"

namespace cirrus::ast {

const ExpressionVtbl BlockExpression::_vtbl{
    /* util::RetainableVtbl */ {
        .retain  = util::_Retainable_vtbl.retain,
        .release = util::_Retainable_vtbl.release,
        .drop =
            [](const util::RetainableVtbl* const vtbl, util::RetainableData* const data) noexcept {
                BlockExpressionData* _data = static_cast<BlockExpressionData*>(data);
                delete _data;
            },
    },
};

BlockExpression BlockExpression::alloc(std::vector<Expression> expressions) noexcept {
    BlockExpressionData* data = new BlockExpressionData{};
    data->expressions         = std::move(expressions);
    return BlockExpression::from_raw(&BlockExpression::_vtbl, data);
}

}  // namespace cirrus::ast
