#pragma once

#include <string_view>

#include "cirrus/util/twine.hpp"

#if !defined(CIRRUS_USE_TWINE)
#define CIRRUS_USE_TWINE 0
#endif  // !defined(CIRRUS_USE_TWINE)

namespace cirrus::util {

#if CIRRUS_USE_TWINE

using String         = util::Twine;
using StringIterator = util::Twine::Iterator;

#else  // CIRRUS_USE_TWINE

using String         = std::string_view;
using StringIterator = size_t;

#endif  // !CIRRUS_USE_TWINE

}  // namespace cirrus::util
