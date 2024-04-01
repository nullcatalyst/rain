#include "rain/lang/target/common/interpreter.hpp"

#include "absl/strings/str_cat.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DerivedTypes.h"
#include "rain/util/assert.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"

namespace llvm {

// These are custom functions added through our custom patches to the LLVM library. They aren't
// declared in any header files, so just declare them for ourselves here.

typedef GenericValue (*ExFunc)(FunctionType*, ArrayRef<GenericValue>);
void InterpreterRegisterExternalFunction(const std::string& name, ExFunc fn);
void InterpreterRemoveExternalFunction(const std::string& name);

}  // namespace llvm

namespace rain::lang {

std::unique_ptr<llvm::ExecutionEngine> create_interpreter(
    std::unique_ptr<llvm::Module>        llvm_module,
    std::unique_ptr<llvm::TargetMachine> llvm_target_machine) {
    llvm::EngineBuilder builder{std::move(llvm_module)};
    std::string         error;
    builder.setErrorStr(&error);
    builder.setEngineKind(llvm::EngineKind::Interpreter);
    auto* engine = builder.create(llvm_target_machine.release());
    IF_DEBUG {
        // This should never happen, but just in case.
        if (engine == nullptr) {
            util::console_error(ANSI_RED, "failed to create interpreter: ", ANSI_RESET, error);
            std::abort();
        }
    }
    return std::unique_ptr<llvm::ExecutionEngine>(engine);
}

namespace {

inline std::string llvm_function_name(const std::string_view function_name) {
    // LLVM for some reason expects this specific prefix for external functions.
    return absl::StrCat("lle_X_", function_name);
}

}  // namespace

void use_interpreter_function(const std::string_view function_name,
                              llvm::GenericValue (*external_function)(
                                  llvm::FunctionType*, llvm::ArrayRef<llvm::GenericValue>)) {
    llvm::InterpreterRegisterExternalFunction(llvm_function_name(function_name), external_function);
}

void remove_interpreter_function(const std::string_view function_name) {
    llvm::InterpreterRemoveExternalFunction(llvm_function_name(function_name));
}

}  // namespace rain::lang
