#pragma once

#include <cstdint>
#include <memory>
#include <span>

#include "rain/buffer.hpp"
#include "rain/util/result.hpp"
#include "rain/util/wasm.hpp"

namespace rain {

util::Result<std::unique_ptr<Buffer>> decompile(const std::span<const uint8_t> wasm);

}  // namespace rain
