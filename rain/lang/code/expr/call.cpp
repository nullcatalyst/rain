#include "rain/lang/ast/expr/call.hpp"

#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/code/expr/any.hpp"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Value* get_element_pointer(Context& ctx, ast::Expression& expression);

llvm::Function* compile_function_declaration(Context&               ctx,
                                             ast::FunctionVariable& function_variable);

llvm::Value* compile_call_method(Context& ctx, ast::Expression& callee,
                                 ast::FunctionVariable&           method,
                                 llvm::ArrayRef<ast::Expression*> arguments) {
    auto&          llvm_ir               = ctx.llvm_builder();
    const uint32_t method_argument_count = method.function_type()->argument_types().size();

    llvm::Value*              self_value = compile_any_expression(ctx, callee);
    std::vector<llvm::Value*> llvm_arguments;
    llvm_arguments.reserve(method_argument_count);

    if (arguments.size() != method_argument_count) {
        // The method takes a self argument.
        llvm_arguments.push_back(self_value);
    }

    for (const auto& argument : arguments) {
        llvm_arguments.emplace_back(compile_any_expression(ctx, *argument));
    }

    if (method_argument_count > 0 && method.function_type()->argument_types()[0] != callee.type()) {
        auto* llvm_self_pointer = get_element_pointer(ctx, callee);
        if (llvm_self_pointer == nullptr) {
            llvm_self_pointer = llvm_ir.CreateAlloca(get_or_compile_type(ctx, *callee.type()));
            llvm_ir.CreateStore(self_value, llvm_self_pointer);
        }
        llvm_arguments[0] = llvm_self_pointer;
    }

    return method.build_call(ctx, llvm_arguments);
}

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
            ast::MemberExpression& member = static_cast<ast::MemberExpression&>(call.callee());

            std::vector<ast::Expression*> arguments;
            arguments.reserve(call.arguments().size());
            for (const auto& argument : call.arguments()) {
                arguments.emplace_back(argument.get());
            }
            return compile_call_method(ctx, member.lhs(), *call.function(), arguments);
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
