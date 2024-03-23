#pragma once

#include "rain/lang/ast/expr/module.hpp"
#include "rain/lang/code/context.hpp"

namespace rain {

void load_external_functions_into_llvm_interpreter();

void add_external_functions_to_module(lang::ast::Module& module);

}  // namespace rain
