#pragma once

#include <memory>
#include <string_view>

#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "rain/buffer.hpp"
#include "rain/lang/code/module.hpp"
#include "rain/util/result.hpp"
#include "rain/util/wasm.hpp"

namespace rain {

util::Result<std::unique_ptr<Buffer>> link(lang::code::Module& module);
util::Result<std::unique_ptr<Buffer>> link(llvm::Module&        llvm_module,
                                           llvm::TargetMachine& llvm_target_machine);
util::Result<std::unique_ptr<Buffer>> link(const std::string_view llvm_ir,
                                           llvm::TargetMachine&   llvm_target_machine);

}  // namespace rain
