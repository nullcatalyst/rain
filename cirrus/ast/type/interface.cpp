#include "cirrus/ast/type/interface.hpp"

namespace cirrus::ast {

const TypeVtbl InterfaceType::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    InterfaceTypeData* _data = static_cast<InterfaceTypeData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::InterfaceType,
    },
};

InterfaceType InterfaceType::alloc(std::string_view                     name,
                                   std::vector<InterfaceTypeMethodData> fields) noexcept {
    InterfaceTypeData* data = new InterfaceTypeData{};
    data->name              = std::move(name);
    data->fields            = std::move(fields);
    return InterfaceType::from_raw(&InterfaceType::_vtbl, data);
}

}  // namespace cirrus::ast
