#pragma once

#include "rain/lang/serial/header.hpp"
#include "rain/lang/serial/packed.hpp"

namespace rain::lang::serial {

PACKED_STRUCT(struct HeaderV0 {
    Header hdr;  // 44 bytes

    uint32_t types_count;
    uint32_t expressions_count;
    uint32_t variables_count;
    uint32_t indices_count;
    uint32_t strings_size;
});
static_assert(sizeof(HeaderV0) == 64);

}  // namespace rain::lang::serial
