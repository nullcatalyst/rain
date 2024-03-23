#include "rain/lib/compile_time_functions.hpp"

#include <cmath>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DerivedTypes.h"
#include "rain/lang/ast/expr/module.hpp"
#include "rain/lang/code/target/interpreter.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"

namespace rain {

namespace {

#define ASSERT_ARGUMENT_COUNT(name, expected_argument_count, got_argument_count)            \
    if (llvm_arguments.size() != expected_argument_count) {                                 \
        rain::util::console_error(ANSI_RED, "bad builtin function call: \"", name,          \
                                  "\": ", ANSI_RESET, "expected ", expected_argument_count, \
                                  " argument", (expected_argument_count == 1 ? "" : "s"),   \
                                  ", got ", got_argument_count);                            \
        std::abort();                                                                       \
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

}  // namespace

void load_external_functions_into_llvm_interpreter() {
    rain::lang::code::target::use_external_function("__builtin_sqrt", lle_X_sqrt);
    rain::lang::code::target::use_external_function("__builtin_cos", lle_X_cos);
    rain::lang::code::target::use_external_function("__builtin_sin", lle_X_sin);
}

void add_external_functions_to_module(lang::ast::Module& module) {
    auto* builtin = module.scope().builtin();
    assert(builtin != nullptr && "builtin scope not found");
    auto* f64 = builtin->f64_type();
    assert(f64 != nullptr && "f64 type not found");

    auto fn_f64__f64 = builtin->get_function_type({f64}, f64);
    assert(fn_f64__f64 != nullptr && "fn (f64) -> f64 function type not found");

    builtin->declare_external_function(std::make_unique<lang::ast::ExternalFunctionVariable>(
        "__builtin_sqrt", fn_f64__f64, "math", "sqrt"));
    builtin->declare_external_function(std::make_unique<lang::ast::ExternalFunctionVariable>(
        "__builtin_cos", fn_f64__f64, "math", "cos"));
    builtin->declare_external_function(std::make_unique<lang::ast::ExternalFunctionVariable>(
        "__builtin_sin", fn_f64__f64, "math", "sin"));
}

}  // namespace rain
