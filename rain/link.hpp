#pragma once

#include <memory>
#include <optional>
#include <string_view>

#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "rain/buffer.hpp"
#include "rain/lang/code/module.hpp"
#include "rain/util/result.hpp"
#include "rain/util/wasm.hpp"

namespace rain {

util::Result<std::unique_ptr<Buffer>> link(lang::code::Module& module, lang::Options& options);
util::Result<std::unique_ptr<Buffer>> link(const std::string_view llvm_ir, lang::Options& options);

}  // namespace rain
