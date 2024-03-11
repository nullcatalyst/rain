#include "rain/crypto/sha256.hpp"

#include <array>
#include <cstring>

namespace rain::crypto::sha256 {

namespace {

constexpr const uint32_t SHA256_BLOCK_SIZE = (512 / 8);

#define SHFR(x, n) (x >> n)
#define ROTR(x, n) ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z) ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHFR(x, 3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

#define UNPACK32(x, str)                                \
    do {                                                \
        *((str) + 3) = static_cast<uint8_t>((x));       \
        *((str) + 2) = static_cast<uint8_t>((x) >> 8);  \
        *((str) + 1) = static_cast<uint8_t>((x) >> 16); \
        *((str) + 0) = static_cast<uint8_t>((x) >> 24); \
    } while (false)

#define PACK32(str, x)                                                                          \
    do {                                                                                        \
        *(x) = (static_cast<uint32_t>(((str)[3]))) | (static_cast<uint32_t>(((str)[2])) << 8) | \
               (static_cast<uint32_t>(((str)[1])) << 16) |                                      \
               (static_cast<uint32_t>(((str)[0])) << 24);                                       \
    } while (false)

#define SHA256_SCR(i)                                                             \
    do {                                                                          \
        w[i] = SHA256_F4(w[i - 2]) + w[i - 7] + SHA256_F3(w[i - 15]) + w[i - 16]; \
    } while (false)

constexpr const std::array<uint32_t, 64> K{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

constexpr const std::array<uint32_t, 8> H0{
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
};

struct Context {
    uint32_t                                   tot_len = 0;
    uint32_t                                   len     = 0;
    std::array<uint8_t, 2 * SHA256_BLOCK_SIZE> block;
    std::array<uint32_t, 8>                    h = H0;
};

void transf(Context& ctx, const uint8_t* message, uint32_t block_nb) {
    uint32_t w[64];
    uint32_t wv[8];

    for (int i = 0; i < static_cast<int>(block_nb); ++i) {
        const uint8_t* const sub_block = message + (i << 6);

        for (int j = 0; j < 16; ++j) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (int j = 16; j < 64; ++j) {
            SHA256_SCR(j);
        }

        for (int j = 0; j < 8; ++j) {
            wv[j] = ctx.h[j];
        }

        for (int j = 0; j < 64; ++j) {
            uint32_t t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6]) + K[j] + w[j];
            uint32_t t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7]       = wv[6];
            wv[6]       = wv[5];
            wv[5]       = wv[4];
            wv[4]       = wv[3] + t1;
            wv[3]       = wv[2];
            wv[2]       = wv[1];
            wv[1]       = wv[0];
            wv[0]       = t1 + t2;
        }

        for (int j = 0; j < 8; ++j) {
            ctx.h[j] += wv[j];
        }
    }
}

void update(Context& ctx, const uint8_t* message, uint32_t len) {
    const uint32_t tmp_len = SHA256_BLOCK_SIZE - ctx.len;
    uint32_t       rem_len = len < tmp_len ? len : tmp_len;

    std::memcpy(&ctx.block[ctx.len], message, rem_len);

    if (ctx.len + len < SHA256_BLOCK_SIZE) {
        ctx.len += len;
        return;
    }

    uint32_t new_len  = len - rem_len;
    uint32_t block_nb = new_len / SHA256_BLOCK_SIZE;

    const uint8_t* const shifted_message = message + rem_len;

    transf(ctx, ctx.block.data(), 1);
    transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA256_BLOCK_SIZE;

    std::memcpy(ctx.block.data(), &shifted_message[block_nb << 6], rem_len);

    ctx.len = rem_len;
    ctx.tot_len += (block_nb + 1) << 6;
}

void final(Context& ctx, uint8_t* digest) {
    const uint32_t block_nb = (1 + ((SHA256_BLOCK_SIZE - 9) < (ctx.len % SHA256_BLOCK_SIZE)));

    const uint32_t len_b  = (ctx.tot_len + ctx.len) << 3;
    const uint32_t pm_len = block_nb << 6;

    std::memset(&ctx.block.data()[ctx.len], 0, pm_len - ctx.len);
    ctx.block[ctx.len] = 0x80;
    UNPACK32(len_b, &ctx.block.data()[pm_len - 4]);

    transf(ctx, ctx.block.data(), block_nb);

    for (int i = 0; i < 8; ++i) {
        UNPACK32(ctx.h[i], &digest[i << 2]);
    }
}

}  // namespace

std::array<uint8_t, SHA256_DIGEST_SIZE> hash(const unsigned char* message, unsigned int len) {
    Context ctx;
    update(ctx, message, len);

    std::array<uint8_t, SHA256_DIGEST_SIZE> digest;
    final(ctx, digest.data());
    return digest;
}

}  // namespace rain::crypto::sha256
