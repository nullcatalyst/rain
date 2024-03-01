#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

#include "rain/util/result.hpp"
#include "rain/util/wasm.hpp"

#if !defined(RAIN_INCLUDE_COMPILE) && !defined(RAIN_INCLUDE_LINK) && \
    !defined(RAIN_INCLUDE_DECOMPILE)
#define RAIN_INCLUDE_COMPILE
#define RAIN_INCLUDE_LINK
#define RAIN_INCLUDE_DECOMPILE
#endif  // !defined(RAIN_INCLUDE_COMPILE) && !defined(RAIN_INCLUDE_LINK) &&
        // !defined(RAIN_INCLUDE_DECOMPILE)

#if defined(RAIN_INCLUDE_COMPILE) || defined(RAIN_INCLUDE_LINK)
#include "rain/code/module.hpp"
#endif  // defined(RAIN_INCLUDE_COMPILE) || defined(RAIN_INCLUDE_LINK)

#if defined(RAIN_INCLUDE_COMPILE)
#include "rain/code/compiler.hpp"
#endif  // defined(RAIN_INCLUDE_COMPILE)

#if defined(RAIN_INCLUDE_LINK)
#include "llvm/IR/Module.h"
#endif  // defined(RAIN_INCLUDE_LINK)

namespace rain {

class Buffer {
  public:
    virtual ~Buffer() = default;

    virtual std::span<const uint8_t> data() const   = 0;
    virtual std::string_view         string() const = 0;
};

#if defined(RAIN_INCLUDE_COMPILE)
util::Result<code::Module> compile(const std::string_view source);
util::Result<code::Module> compile(const std::string_view source,
                                   void (*init_compiler)(code::Compiler&));
#endif  // defined(RAIN_INCLUDE_COMPILE)

#if defined(RAIN_INCLUDE_LINK)
util::Result<std::unique_ptr<Buffer>> link(code::Module& module);
util::Result<std::unique_ptr<Buffer>> link(llvm::Module& module);
util::Result<std::unique_ptr<Buffer>> link(const std::string_view llvm_ir);
#endif  // defined(RAIN_INCLUDE_LINK)

#if defined(RAIN_INCLUDE_DECOMPILE)
util::Result<std::unique_ptr<Buffer>> decompile(const std::span<const uint8_t> wasm);
#endif  // defined(RAIN_INCLUDE_DECOMPILE)

}  // namespace rain
