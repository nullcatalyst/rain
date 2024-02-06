#include "cirrus/ast/type/opaque.hpp"

namespace cirrus::ast {

const TypeVtbl OpaqueType::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    OpaqueTypeData* _data = static_cast<OpaqueTypeData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::OpaqueType,
    },
};

OpaqueType OpaqueType::alloc(std::string_view name) noexcept {
    OpaqueTypeData* data = new OpaqueTypeData{};
    data->name           = std::move(name);
    return OpaqueType::from_raw(&OpaqueType::_vtbl, data);
}

}  // namespace cirrus::ast
