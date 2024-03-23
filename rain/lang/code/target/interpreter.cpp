#include "rain/lang/code/target/interpreter.hpp"

#include "absl/strings/str_cat.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DerivedTypes.h"
#include "rain/lang/err/simple.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/result.hpp"

namespace llvm {

// These are custom functions added through our custom patches to the LLVM library. They aren't
// declared in any header files, so just declare them for ourselves here.

typedef GenericValue (*ExFunc)(FunctionType*, ArrayRef<GenericValue>);
void InterpreterRegisterExternalFunction(const std::string& name, ExFunc fn);
void InterpreterRemoveExternalFunction(const std::string& name);

}  // namespace llvm

namespace rain::lang::code::target {

util::Result<std::unique_ptr<llvm::ExecutionEngine>> create_interpreter(
    std::unique_ptr<llvm::Module>        llvm_module,
    std::unique_ptr<llvm::TargetMachine> llvm_target_machine) {
    llvm::EngineBuilder builder{std::move(llvm_module)};
    std::string         error;
    builder.setErrorStr(&error);
    builder.setEngineKind(llvm::EngineKind::Interpreter);
    auto* engine = builder.create(llvm_target_machine.release());
    if (engine == nullptr) {
        return ERR_PTR(err::SimpleError, absl::StrCat("failed to create interpreter: ", error));
    }
    return std::unique_ptr<llvm::ExecutionEngine>(engine);
}

void use_external_function(const std::string_view function_name,
                           llvm::GenericValue (*external_function)(
                               llvm::FunctionType*, llvm::ArrayRef<llvm::GenericValue>)) {
    llvm::InterpreterRegisterExternalFunction(absl::StrCat("lle_X_", function_name),
                                              external_function);
}

}  // namespace rain::lang::code::target
