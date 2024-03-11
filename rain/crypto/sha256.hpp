#pragma once

#include <array>
#include <cstdint>

namespace rain::crypto::sha256 {

constexpr const uint32_t SHA256_DIGEST_SIZE = (256 / 8);  // 256 bits, 8 bits per byte == 32 bytes

using Digest = std::array<uint8_t, SHA256_DIGEST_SIZE>;

Digest hash(const uint8_t* message, uint32_t len);

}  // namespace rain::crypto::sha256
