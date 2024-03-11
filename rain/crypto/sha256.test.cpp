#include "rain/crypto/sha256.hpp"

#include <gtest/gtest.h>

TEST(SHA256, hello) {
    using namespace rain::crypto::sha256;

    // Ran on macos:
    // echo -n hello | shasum -a 256
    // 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824  -

    constexpr std::array<uint8_t, 32> EXPECTED_DIGEST{
        // clang-format off
        0x2c, 0xf2, 0x4d, 0xba, 0x5f, 0xb0, 0xa3, 0x0e,
        0x26, 0xe8, 0x3b, 0x2a, 0xc5, 0xb9, 0xe2, 0x9e,
        0x1b, 0x16, 0x1e, 0x5c, 0x1f, 0xa7, 0x42, 0x5e,
        0x73, 0x04, 0x33, 0x62, 0x93, 0x8b, 0x98, 0x24,
        // clang-format on
    };

    const auto digest = sha256(reinterpret_cast<const uint8_t*>("hello"), 5);
    EXPECT_EQ(digest, EXPECTED_DIGEST);
}
