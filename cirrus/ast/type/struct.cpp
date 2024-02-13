#include "cirrus/ast/type/struct.hpp"

namespace cirrus::ast {

const TypeVtbl StructType::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    StructTypeData* _data = static_cast<StructTypeData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::StructType,
    },
};

StructType StructType::alloc(std::optional<util::Twine>       name,
                             std::vector<StructTypeFieldData> fields) noexcept {
    StructTypeData* data = new StructTypeData{};
    data->name           = name;
    data->fields         = std::move(fields);
    return StructType::from_raw(&StructType::_vtbl, data);
}

}  // namespace cirrus::ast
