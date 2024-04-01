#pragma once

#include <memory>
#include <string_view>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

typedef GenericValue (*ExFunc)(FunctionType*, ArrayRef<GenericValue>);

}  // namespace llvm

namespace rain::lang {

std::unique_ptr<llvm::ExecutionEngine> create_interpreter(
    std::unique_ptr<llvm::Module>        llvm_module,
    std::unique_ptr<llvm::TargetMachine> llvm_target_machine);

void use_interpreter_function(const std::string_view function_name, llvm::ExFunc external_function);
void remove_interpreter_function(const std::string_view function_name);

}  // namespace rain::lang
