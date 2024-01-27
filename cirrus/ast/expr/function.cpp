#include "cirrus/ast/expr/function.hpp"

namespace cirrus::ast {

const ExpressionVtbl FunctionExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    FunctionExpressionData* _data = static_cast<FunctionExpressionData*>(data);
                    delete _data;
                },
        },
        .kind = NodeKind::FunctionExpression,
    },
};

FunctionExpression FunctionExpression::alloc(std::optional<std::string_view>   name,
                                             std::optional<Type>               return_type,
                                             std::vector<FunctionArgumentData> arguments,
                                             std::vector<Expression> expressions) noexcept {
    FunctionExpressionData* data = new FunctionExpressionData{};
    data->name                   = std::move(name);
    data->return_type            = std::move(return_type);
    data->arguments              = std::move(arguments);
    data->expressions            = std::move(expressions);
    return FunctionExpression::from_raw(&FunctionExpression::_vtbl, data);
}

}  // namespace cirrus::ast
