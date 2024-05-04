#include "llvm/IR/Value.h"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/ast/var/unwrapped_optional.hpp"
#include "rain/lang/code/expr/any.hpp"

namespace rain::lang::code {

llvm::Value* get_element_pointer(Context& ctx, ast::Expression& expression) {
    switch (expression.kind()) {
        case serial::ExpressionKind::Variable: {
            auto& identifier = static_cast<ast::IdentifierExpression&>(expression);
            auto* variable   = identifier.variable();

            if (variable->kind() == serial::VariableKind::UnwrappedOptional) {
                auto* unwrapped_optional = static_cast<ast::UnwrappedOptionalVariable*>(variable);
                auto* llvm_value_ptr     = ctx.llvm_value(unwrapped_optional->variable());
                return ctx.llvm_builder().CreateStructGEP(ctx.llvm_type(unwrapped_optional->type()),
                                                          llvm_value_ptr, 0);
            }

            if (variable->mutable_() || variable->type()->kind() == serial::TypeKind::Reference) {
                return ctx.llvm_value(variable);
            }

            // The variable is not mutable, so it may only exist in a register. We have no memory
            // location to load it from.
            return nullptr;
        }

        case serial::ExpressionKind::Member: {
            auto& member     = static_cast<ast::MemberExpression&>(expression);
            auto* owner_type = member.lhs().type();
            if (owner_type->kind() == serial::TypeKind::Reference) {
                owner_type = &static_cast<ast::ReferenceType&>(*owner_type).type();
            }
            assert(owner_type->kind() == serial::TypeKind::Struct);

            auto* llvm_owner_ptr = get_element_pointer(ctx, member.lhs());
            if (llvm_owner_ptr == nullptr) {
                return nullptr;
            }

            auto* llvm_owner_type = ctx.llvm_type(owner_type);
            auto  member_index =
                static_cast<ast::StructType&>(*owner_type).find_member(member.name()).value();
            return ctx.llvm_builder().CreateStructGEP(llvm_owner_type, llvm_owner_ptr,
                                                      member_index);
        }

        default: {
            if (expression.type()->kind() != serial::TypeKind::Reference) {
                // Error: cannot take the address of a temporary value.
                return nullptr;
            }

            return compile_any_expression(ctx, expression);
        }
    }
}

}  // namespace rain::lang::code
