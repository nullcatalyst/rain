#pragma once

#include "cirrus/ast/node.hpp"
#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct Expression;
struct ExpressionData;

struct ExpressionVtbl : public NodeVtbl {
    [[nodiscard]] bool (*compile_time_able)(const ExpressionVtbl* const vtbl,
                                            ExpressionData* const       data) noexcept = nullptr;
};

struct ExpressionData : public NodeData {
    Type _type;
};

template <typename This, typename Vtbl, typename Data>
struct IExpression : public INode<This, Vtbl, Data> {
    using INode<This, Vtbl, Data>::INode;

    [[nodiscard]] constexpr const Type& type() const noexcept { return this->_data->_type; }
    [[nodiscard]] constexpr bool        compile_time_able() const noexcept {
        return this->_vtbl->compile_time_able(this->_vtbl, this->_data);
    }
};

struct Expression : public IExpression<Expression, ExpressionVtbl, ExpressionData> {
    using IExpression<Expression, ExpressionVtbl, ExpressionData>::IExpression;
};

#define DECLARE_EXPRESSION(Name) \
    struct Name##Expression      \
        : public IExpression<Name##Expression, ExpressionVtbl, Name##ExpressionData>

#define EXPRESSION_COMMON_IMPL(Name)                                                        \
    using IExpression<Name##Expression, ExpressionVtbl, Name##ExpressionData>::IExpression; \
                                                                                            \
    static const ExpressionVtbl _vtbl;                                                      \
                                                                                            \
    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }

#define EXPRESSION_VTBL_COMMON_IMPL(Name)                                                   \
    /* Node */ {                                                                            \
        /* util::RetainableVtbl */ {                                                        \
            .retain  = util::_Retainable_vtbl.retain,                                       \
            .release = util::_Retainable_vtbl.release,                                      \
            .drop =                                                                         \
                [](const util::RetainableVtbl* const vtbl,                                  \
                   util::RetainableData* const       data) noexcept {                             \
                    Name##ExpressionData* _data = static_cast<Name##ExpressionData*>(data); \
                    delete _data;                                                           \
                },                                                                          \
        },                                                                                  \
            /*.kind = */ NodeKind::Name##Expression,                                        \
    }

}  // namespace cirrus::ast
