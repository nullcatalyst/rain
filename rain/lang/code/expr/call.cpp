#include "rain/lang/ast/expr/call.hpp"

#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/code/expr/any.hpp"

namespace rain::lang::code {

llvm::Function* compile_function_declaration(Context&               ctx,
                                             ast::FunctionVariable& function_variable);

llvm::Value* compile_call(Context& ctx, ast::CallExpression& call) {
    const auto get_arguments = [&](llvm::Value* self_value =
                                       nullptr) -> llvm::SmallVector<llvm::Value*, 4> {
        llvm::SmallVector<llvm::Value*, 4> arguments;
        arguments.reserve(call.arguments().size() + 1);

        if (self_value != nullptr) {
            arguments.push_back(self_value);
        }
        for (const auto& argument : call.arguments()) {
            arguments.push_back(compile_any_expression(ctx, *argument));
        }
        return arguments;
    };

    auto& ir = ctx.llvm_builder();

    switch (call.callee().kind()) {
        case serial::ExpressionKind::Member: {
            ast::MemberExpression& member     = static_cast<ast::MemberExpression&>(call.callee());
            llvm::Value*           self_value = compile_any_expression(ctx, member.lhs());
            ast::FunctionVariable* function   = call.function();

            if (function->function_type()->argument_types().size() == call.arguments().size()) {
                const auto arguments = get_arguments();
                return function->build_call(ctx, arguments);
            } else if (function->function_type()->argument_types().size() ==
                       call.arguments().size() + 1) {
                // The LLVM function type having 1 more argument than the function type means
                // that the function expects to be passed the self value.
                const auto arguments = get_arguments(self_value);
                return function->build_call(ctx, arguments);
            } else {
                util::panic(
                    "failed to compile function call: internal error: unknown number of expected "
                    "arguments");
            }
        }

        case serial::ExpressionKind::Variable: {
            if (ast::FunctionVariable* function = call.function(); call.function() != nullptr) {
                llvm::FunctionType* llvm_function_type =
                    reinterpret_cast<llvm::FunctionType*>(ctx.llvm_type(function->function_type()));
                llvm::Value* llvm_function = ctx.llvm_value(function);

                if (llvm_function == nullptr) {
                    llvm_function = compile_function_declaration(ctx, *function);
                }

                const auto arguments = get_arguments();
                return ir.CreateCall(llvm_function_type, llvm_function, arguments);
            }

            // The identifier is not referring to a function, so treat it as a variable that has a
            // callable type.
            [[fallthrough]];
        }

        default:
            std::abort();
            break;
    }
}

}  // namespace rain::lang::code
