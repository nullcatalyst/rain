#include "rain/lang/target/wasm/options.hpp"

#include <array>
#include <cstdlib>
#include <span>
#include <string_view>
#include <tuple>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/MC/TargetRegistry.h"
#include "rain/lang/code/context.hpp"
#include "rain/lang/target/common/interpreter.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"

namespace rain::lang::wasm {

namespace {

#define ASSERT_ARGUMENT_COUNT(name, expected_argument_count, got_argument_count)            \
    if (llvm_arguments.size() != expected_argument_count) {                                 \
        rain::util::console_error(ANSI_RED, "bad builtin function call: \"", name,          \
                                  "\": ", ANSI_RESET, "expected ", expected_argument_count, \
                                  " argument", (expected_argument_count == 1 ? "" : "s"),   \
                                  ", got ", got_argument_count);                            \
        std::abort();                                                                       \
    }

llvm::GenericValue lle_X_atan2(llvm::FunctionType*                llvm_function_type,
                               llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("atan2", 2, llvm_arguments.size());

    llvm::GenericValue result;
    result.FloatVal = std::atan2(llvm_arguments[0].FloatVal, llvm_arguments[1].FloatVal);
    return result;
}

llvm::GenericValue lle_X_cos(llvm::FunctionType*                llvm_function_type,
                             llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("cos", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.FloatVal = std::cos(llvm_arguments[0].FloatVal);
    return result;
}

llvm::GenericValue lle_X_sin(llvm::FunctionType*                llvm_function_type,
                             llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("sin", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.FloatVal = std::sin(llvm_arguments[0].FloatVal);
    return result;
}

llvm::GenericValue lle_X_sqrt(llvm::FunctionType*                llvm_function_type,
                              llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("sqrt", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.FloatVal = std::sqrt(llvm_arguments[0].FloatVal);
    return result;
}

llvm::GenericValue lle_X_tan(llvm::FunctionType*                llvm_function_type,
                             llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    ASSERT_ARGUMENT_COUNT("tan", 1, llvm_arguments.size());

    llvm::GenericValue result;
    result.FloatVal = std::tan(llvm_arguments[0].FloatVal);
    return result;
}

#undef ASSERT_ARGUMENT_COUNT

using ExternFunction  = std::tuple<const std::string_view, const llvm::ExFunc>;
using ExternNamespace = std::tuple<const std::string_view, std::span<const ExternFunction>>;

static constexpr const std::array<ExternFunction, 5> MATH_FUNCTIONS{
    // clang-format off
    // <keep_sorted>
    ExternFunction{"atan2", lle_X_atan2},
    ExternFunction{"cos", lle_X_cos},
    ExternFunction{"sin", lle_X_sin},
    ExternFunction{"sqrt", lle_X_sqrt},
    ExternFunction{"tan", lle_X_tan},
    // </keep_sorted>
    // clang-format on
};

static constexpr const std::array<ExternNamespace, 1> EXTERN_NAMESPACES{
    // clang-format off
    // <keep_sorted>
    ExternNamespace{"math", MATH_FUNCTIONS},
    // </keep_sorted>
    // clang-format on
};

llvm::ExFunc find_extern_function(const std::string_view namespace_name,
                                  const std::string_view function_name) {
    const auto found_namespace =
        std::lower_bound(EXTERN_NAMESPACES.begin(), EXTERN_NAMESPACES.end(), namespace_name,
                         [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });
    if (found_namespace == EXTERN_NAMESPACES.end()) {
        return nullptr;
    }

    const auto& [namespace_name_, functions] = *found_namespace;
    const auto found_function =
        std::lower_bound(functions.begin(), functions.end(), function_name,
                         [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });
    if (found_function == functions.end()) {
        return nullptr;
    }

    return std::get<1>(*found_function);
}

}  // namespace

Options::~Options() {
    for (const auto& function : _interpreter_functions) {
        remove_interpreter_function(function);
    }
}

std::unique_ptr<llvm::TargetMachine> Options::create_target_machine() {
    std::string error;

    std::string         target_triple = llvm::Triple::normalize("wasm32-unknown-unknown");
    const llvm::Target* target        = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (target == nullptr) {
        util::console_error(ANSI_RED, "failed to lookup target: ", ANSI_RESET, error);
        std::abort();
    }

    std::string cpu;
    std::string features =
        "+simd128,+sign-ext,+bulk-memory,+mutable-globals,+nontrapping-fptoint,+multivalue";
    return std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(target_triple, cpu, features, llvm::TargetOptions(),
                                    std::nullopt, std::nullopt, llvm::CodeGenOpt::Default));
}

std::unique_ptr<llvm::ExecutionEngine> Options::create_engine(
    std::unique_ptr<llvm::Module>        llvm_module,
    std::unique_ptr<llvm::TargetMachine> llvm_target_machine) {
    return create_interpreter(std::move(llvm_module), std::move(llvm_target_machine));
}

bool Options::extern_is_compile_time_runnable(const std::span<const std::string> keys) {
    if (keys.size() != 3 || keys[0] != "js") {
        return false;
    }

    return find_extern_function(keys[1], keys[2]) != nullptr;
}

void Options::compile_extern_compile_time_runnable(code::Context&                     ctx,
                                                   llvm::Function*                    llvm_function,
                                                   const std::span<const std::string> keys) {
    const std::string& function_name = llvm_function->getName().str();
    _interpreter_functions.emplace_back(function_name);

    llvm::ExFunc fn = find_extern_function(keys[1], keys[2]);
    use_interpreter_function(function_name, fn);

    llvm_function->addFnAttr(
        llvm::Attribute::get(ctx.llvm_context(), "wasm-import-module", keys[1]));
    llvm_function->addFnAttr(llvm::Attribute::get(ctx.llvm_context(), "wasm-import-name", keys[2]));
}

}  // namespace rain::lang::wasm
