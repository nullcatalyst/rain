#pragma once

#include "rain/lang/code/context.hpp"

// Expressions
#include "rain/lang/ast/expr/binary_operator.hpp"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/call.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/expr/module.hpp"
#include "rain/lang/ast/expr/type.hpp"

namespace rain::lang::code {

void compile_module(Context& ctx, ast::Module& module);

// Expressions
llvm::Value* compile_any_expression(Context& ctx, ast::Expression& expression);

llvm::Value* compile_integer(Context& ctx, ast::IntegerExpression& integer);
llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier);
llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator);
llvm::Value* compile_member(Context& ctx, ast::MemberExpression& member);
llvm::Value* compile_call(Context& ctx, ast::CallExpression& call);
llvm::Value* compile_block(Context& ctx, ast::BlockExpression& block);
llvm::Function* compile_function(Context& ctx, ast::FunctionExpression& function);

// Types
llvm::Type* compile_type(Context& ctx, ast::Type& type);

}  // namespace rain::lang::code
