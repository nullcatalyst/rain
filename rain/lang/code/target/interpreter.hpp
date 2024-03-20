#pragma once

#include <memory>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "rain/util/result.hpp"

namespace rain::lang::code::target {

util::Result<std::unique_ptr<llvm::ExecutionEngine>> create_interpreter(
    std::unique_ptr<llvm::Module>        llvm_module,
    std::unique_ptr<llvm::TargetMachine> llvm_target_machine);

void use_external_function(const std::string_view function_name,
                           llvm::GenericValue (*external_function)(
                               llvm::FunctionType*, llvm::ArrayRef<llvm::GenericValue>));

}  // namespace rain::lang::code::target
