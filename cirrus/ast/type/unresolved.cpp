#include "cirrus/ast/type/unresolved.hpp"

namespace cirrus::ast {

const TypeVtbl UnresolvedType::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    UnresolvedTypeData* _data = static_cast<UnresolvedTypeData*>(data);
                    delete _data;
                },
        },
        .kind = NodeKind::UnresolvedType,
    },
};

UnresolvedType UnresolvedType::alloc(std::string_view name) noexcept {
    UnresolvedTypeData* data = new UnresolvedTypeData{};
    data->name               = name;
    return UnresolvedType::from_raw(&UnresolvedType::_vtbl, data);
}

}  // namespace cirrus::ast
