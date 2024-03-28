#pragma once

#include <cassert>

#if !defined(NDEBUG)

#define IF_DEBUG if constexpr (true)

#else  // !defined(NDEBUG)

#define IF_DEBUG if constexpr (false)

#endif  // defined(NDEBUG)
