#pragma once

#if !defined(PACKED_STRUCT)
#if defined(__GNUC__) || defined(__clang__)
#define PACKED_STRUCT(...) __VA_ARGS__ __attribute__((__packed__));
#elif defined(_MSC_VER)
#define PACKED_STRUCT(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
#define PACKED_STRUCT(...) __VA_ARGS__
#endif
#endif  // !defined(PACKED_STRUCT)
