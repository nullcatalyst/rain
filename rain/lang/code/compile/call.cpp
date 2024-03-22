#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_call(Context& ctx, ast::CallExpression& call) {
    const auto get_arguments = [&](llvm::Value* self_value = nullptr) -> std::vector<llvm::Value*> {
        std::vector<llvm::Value*> arguments;
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

    switch (call.callee()->kind()) {
        case serial::ExpressionKind::Member: {
            ast::MemberExpression& member     = static_cast<ast::MemberExpression&>(*call.callee());
            llvm::Value*           self_value = compile_any_expression(ctx, *member.lhs());
            ast::FunctionVariable* function   = call.function();
            llvm::FunctionType*    llvm_function_type =
                reinterpret_cast<llvm::FunctionType*>(ctx.llvm_type(function->function_type()));
            llvm::Value* llvm_function = ctx.llvm_value(function);

            if (function->function_type()->argument_types().size() == call.arguments().size()) {
                const auto arguments = get_arguments();
                return ir.CreateCall(llvm_function_type, llvm_function, arguments);
            } else if (function->function_type()->argument_types().size() ==
                       call.arguments().size() + 1) {
                // The LLVM function type having 1 more argument than the function type means
                // that the function expects to be passed the self value.
                const auto arguments = get_arguments(self_value);
                return ir.CreateCall(llvm_function_type, llvm_function, arguments);
            } else {
                util::console_error(
                    "failed to compile function call: internal error: unknown number of expected "
                    "arguments");
                std::abort();
            }
        }

        case serial::ExpressionKind::Variable: {
            if (call.function() != nullptr) {
                ast::FunctionVariable* function = call.function();
                llvm::FunctionType*    llvm_function_type =
                    reinterpret_cast<llvm::FunctionType*>(ctx.llvm_type(function->function_type()));
                llvm::Value* llvm_function = ctx.llvm_value(function);

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
