#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace rain {

class Buffer {
  public:
    virtual ~Buffer() = default;

    virtual std::span<const uint8_t> data() const   = 0;
    virtual std::string_view         string() const = 0;
};

}  // namespace rain
