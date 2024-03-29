#pragma once

#include <string_view>

#include "llvm/IR/Value.h"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

class ExternalFunctionVariable : public FunctionVariable {
    std::string_view _wasm_namespace;
    std::string_view _wasm_function_name;

  public:
    ExternalFunctionVariable(std::string_view name, FunctionType* type,
                             std::string_view wasm_namespace, std::string_view wasm_function_name)
        : FunctionVariable(name, std::move(type)),
          _wasm_namespace(wasm_namespace),
          _wasm_function_name(wasm_function_name) {}
    ~ExternalFunctionVariable() override = default;

    [[nodiscard]] std::string_view wasm_namespace() const noexcept { return _wasm_namespace; }
    [[nodiscard]] std::string_view wasm_function_name() const noexcept {
        return _wasm_function_name;
    }
};

}  // namespace rain::lang::ast
