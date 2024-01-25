#include "cirrus/util/retain.hpp"

namespace cirrus::util {

const RetainableVtbl _Retainable_vtbl{
    .retain = [](const RetainableVtbl* const vtbl,
                 RetainableData* const       data) noexcept { ++data->ref_count; },
    .release =
        [](const RetainableVtbl* const vtbl, RetainableData* const data) noexcept {
            if (--data->ref_count == 0) {
                vtbl->drop(vtbl, data);
            }
        },
    .drop = [](const RetainableVtbl* const vtbl,
               RetainableData* const       data) noexcept { delete data; },
};

}  // namespace cirrus::util
