#pragma once

#include <array>
#include <cstdint>

#include "rain/crypto/sha256.hpp"
#include "rain/lang/serial/packed.hpp"

namespace rain::lang::serial {

using MagicNumber = std::array<uint8_t, 8>;

PACKED_STRUCT(struct Header {
    MagicNumber magic;

    /**
     * The SHA-256 hash of the library.
     *
     * This is used to verify the integrity of the library.
     * It is calculated by hashing the entire library, starting from the `version` field.
     */
    crypto::sha256::Digest sha256;

    uint8_t                version;
    std::array<uint8_t, 3> _reserved_1;  // Padding to end the struct on a 4 byte boundary
});

constexpr const MagicNumber RAIN_MAGIC_NUMBER{
    // clang-format off
    // '\0rainlib' == 0x62696c6e61726f00
    0x00, 0x6f, 0x72, 0x61, 0x6e, 0x6c, 0x69, 0x62,
    // clang-format on
};

}  // namespace rain::lang::serial
