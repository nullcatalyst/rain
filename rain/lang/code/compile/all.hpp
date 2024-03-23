#pragma once

#include "rain/lang/code/context.hpp"

// Expressions
#include "rain/lang/ast/expr/binary_operator.hpp"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/call.hpp"
#include "rain/lang/ast/expr/compile_time.hpp"
#include "rain/lang/ast/expr/export.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/float.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/if.hpp"
#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/expr/module.hpp"
#include "rain/lang/ast/expr/parenthesis.hpp"
#include "rain/lang/ast/expr/type.hpp"
#include "rain/lang/ast/expr/unary_operator.hpp"

namespace rain::lang::code {

void compile_module(Context& ctx, ast::Module& module);

// Expressions
llvm::Value* compile_any_expression(Context& ctx, ast::Expression& expression);

llvm::Value* compile_integer(Context& ctx, ast::IntegerExpression& integer);
llvm::Value* compile_float(Context& ctx, ast::FloatExpression& float_);
llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier);
llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator);
llvm::Value* compile_unary_operator(Context& ctx, ast::UnaryOperatorExpression& unary_operator);
llvm::Value* compile_member(Context& ctx, ast::MemberExpression& member);
llvm::Value* compile_call(Context& ctx, ast::CallExpression& call);
llvm::Value* compile_compile_time(Context& ctx, ast::CompileTimeExpression& compile_time);
llvm::Value* compile_parenthesis(Context& ctx, ast::ParenthesisExpression& parenthesis);
llvm::Value* compile_block(Context& ctx, ast::BlockExpression& block);
llvm::Function* compile_function(Context& ctx, ast::FunctionExpression& function);
llvm::Value*    compile_if(Context& ctx, ast::IfExpression& if_);
llvm::Value*    compile_export(Context& ctx, ast::ExportExpression& export_);

// Types
llvm::Type* compile_type(Context& ctx, ast::Type& type);

}  // namespace rain::lang::code
