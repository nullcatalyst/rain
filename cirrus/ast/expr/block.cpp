#include "cirrus/ast/expr/block.hpp"

namespace cirrus::ast {

const ExpressionVtbl BlockExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Block),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const BlockExpressionData* _data = static_cast<BlockExpressionData*>(data);
            for (const Expression& expr : _data->expressions) {
                if (!expr.compile_time_able()) {
                    return false;
                }
            }
            return true;
        },
};

BlockExpression BlockExpression::alloc(std::vector<Expression> expressions) noexcept {
    BlockExpressionData* data = new BlockExpressionData{};
    data->expressions         = std::move(expressions);
    return BlockExpression::from_raw(&BlockExpression::_vtbl, data);
}

}  // namespace cirrus::ast
