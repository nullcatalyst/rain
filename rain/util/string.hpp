#pragma once

#include <string_view>

#include "rain/util/twine.hpp"

#if !defined(RAIN_USE_TWINE)
#define RAIN_USE_TWINE 0
#endif  // !defined(RAIN_USE_TWINE)

namespace rain::util {

#if RAIN_USE_TWINE

using String         = util::Twine;
using StringIterator = util::Twine::Iterator;

#else  // RAIN_USE_TWINE

using String         = std::string_view;
using StringIterator = ssize_t;

#endif  // !RAIN_USE_TWINE

}  // namespace rain::util
