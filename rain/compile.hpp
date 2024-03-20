#pragma once

#include <string_view>

#include "llvm/ADT/STLFunctionalExtras.h"
#include "rain/lang/code/module.hpp"

namespace rain {

util::Result<lang::code::Module> compile(const std::string_view source);
util::Result<lang::code::Module> compile(const std::string_view     source,
                                         llvm::function_ref<void()> init_compiler);

}  // namespace rain
