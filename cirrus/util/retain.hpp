#pragma once

#include <cstdint>
#include <iostream>

#include "cirrus/util/ptr.hpp"

namespace cirrus::util {

struct RetainableData {
    uint32_t ref_count = 0;
};

struct RetainableVtbl {
    void (*retain)(const RetainableVtbl* const vtbl, RetainableData* const data) noexcept;
    void (*release)(const RetainableVtbl* const vtbl, RetainableData* const data) noexcept;
    void (*drop)(const RetainableVtbl* const vtbl, RetainableData* const data) noexcept;
};

template <typename Vtbl, typename Data>
struct RetainMemMgr {
    static constexpr void retain(const Vtbl* vtbl, Data* data) noexcept {
        // std::cout << "Retaining " << data
        //           << " (pre ref_count = " << (data != nullptr ? data->ref_count : 0) << ")"
        //           << std::endl;
        if (data != nullptr) {
            vtbl->retain(vtbl, data);
        }
    }

    static constexpr void release(const Vtbl* vtbl, Data* data) noexcept {
        // std::cout << "Releasing " << data
        //           << " (pre ref_count = " << (data != nullptr ? data->ref_count : 0) << ")"
        //           << std::endl;
        if (data != nullptr) {
            vtbl->release(vtbl, data);
        }
    }
};

template <typename This, typename Vtbl, typename Data>
struct Retainable : public Ptr<This, Vtbl, Data, RetainMemMgr<RetainableVtbl, RetainableData>> {
    static_assert(std::is_base_of_v<RetainableVtbl, Vtbl>,
                  "Vtbl must be derived from RetainableVtbl");
    static_assert(std::is_base_of_v<RetainableData, Data>,
                  "Data must be derived from RetainableData");

    using Ptr<This, Vtbl, Data, RetainMemMgr<RetainableVtbl, RetainableData>>::Ptr;

    constexpr uint32_t ref_count() const noexcept { return this->_data->ref_count; }
};

extern const RetainableVtbl _Retainable_vtbl;

}  // namespace cirrus::util
