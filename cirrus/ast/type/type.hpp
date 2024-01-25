#pragma once

#include <string_view>

#include "cirrus/util/retain.hpp"

namespace cirrus::ast {

struct TypeVtbl : public util::RetainableVtbl {};

struct TypeData : public util::RetainableData {};

template <typename This, typename Vtbl, typename Data>
struct IType : public util::Retainable<This, Vtbl, Data> {
    using util::Retainable<This, Vtbl, Data>::Retainable;
};

struct Type : public IType<Type, TypeVtbl, TypeData> {
    using IType<Type, TypeVtbl, TypeData>::IType;
};

}  // namespace cirrus::ast
