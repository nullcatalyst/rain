#pragma once

#include <cmath>

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DerivedTypes.h"
#include "rain/code/compiler.hpp"

namespace {

#define ASSERT_ARGUMENT_COUNT(name, expected, got)                                        \
    if (llvm_arguments.size() != 1) {                                                     \
        rain::util::console_error(ANSI_RED, "bad builtin function call: \"", name,        \
                                  "\": ", ANSI_RESET, "expected ", expected, " argument", \
                                  (expected == 1 ? "" : "s"), ", got ", got);             \
        std::abort();                                                                     \
    }

llvm::GenericValue lle_X_sqrt(llvm::FunctionType*                llvm_function_type,
                              llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("sqrt", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.DoubleVal = std::sqrt(llvm_arguments[0].DoubleVal);
    return result;
}

llvm::GenericValue lle_X_cos(llvm::FunctionType*                llvm_function_type,
                             llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("cos", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.DoubleVal = std::cos(llvm_arguments[0].DoubleVal);
    return result;
}

llvm::GenericValue lle_X_sin(llvm::FunctionType*                llvm_function_type,
                             llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("sin", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.DoubleVal = std::sin(llvm_arguments[0].DoubleVal);
    return result;
}

#undef ASSERT_ARGUMENT_COUNT

void load_external_functions_into_llvm() {
    rain::code::Compiler::use_external_function("__builtin_sqrt", lle_X_sqrt);
    rain::code::Compiler::use_external_function("__builtin_cos", lle_X_cos);
    rain::code::Compiler::use_external_function("__builtin_sin", lle_X_sin);
}

void add_external_functions_to_compiler(rain::code::Compiler& compiler) {
    const auto& scope = compiler.builtin_scope();
    const auto  f64   = scope.find_named_type("f64");
    assert(f64 != nullptr && "f64 type not found");

    auto fn_f64__f64_result = compiler.get_function_type({f64}, f64);
    assert(fn_f64__f64_result.has_value() && "fn (f64) -> f64 function type not found");
    const auto fn_f64__f64 = std::move(fn_f64__f64_result).value();

    compiler.declare_external_function("math", "sqrt", "__builtin_sqrt", fn_f64__f64);
    compiler.declare_external_function("math", "cos", "__builtin_cos", fn_f64__f64);
    compiler.declare_external_function("math", "sin", "__builtin_sin", fn_f64__f64);
}

}  // namespace
