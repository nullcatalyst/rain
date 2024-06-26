#pragma once

#include "rain/lang/code/context.hpp"
#include "rain/lang/code/expr/any.hpp"

// Expressions
#include "rain/lang/ast/expr/array_literal.hpp"
#include "rain/lang/ast/expr/binary_operator.hpp"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/boolean.hpp"
#include "rain/lang/ast/expr/call.hpp"
#include "rain/lang/ast/expr/cast.hpp"
#include "rain/lang/ast/expr/compile_time.hpp"
#include "rain/lang/ast/expr/export.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/extern.hpp"
#include "rain/lang/ast/expr/float.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/if.hpp"
#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/ast/expr/let.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/expr/parenthesis.hpp"
#include "rain/lang/ast/expr/slice_literal.hpp"
#include "rain/lang/ast/expr/string.hpp"
#include "rain/lang/ast/expr/struct_literal.hpp"
#include "rain/lang/ast/expr/type.hpp"
#include "rain/lang/ast/expr/unary_operator.hpp"
#include "rain/lang/ast/expr/while.hpp"
#include "rain/lang/ast/module.hpp"

namespace rain::lang::code {

void compile_module(Context& ctx, ast::Module& module);

// Expressions
llvm::Value* compile_boolean(Context& ctx, ast::BooleanExpression& boolean);
llvm::Value* compile_integer(Context& ctx, ast::IntegerExpression& integer);
llvm::Value* compile_float(Context& ctx, ast::FloatExpression& float_);
llvm::Value* compile_string(Context& ctx, ast::StringExpression& string_);
llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier);
llvm::Value* compile_struct_literal(Context& ctx, ast::StructLiteralExpression& struct_literal);
llvm::Value* compile_array_literal(Context& ctx, ast::ArrayLiteralExpression& array_literal);
llvm::Value* compile_slice_literal(Context& ctx, ast::SliceLiteralExpression& slice_literal);
llvm::Value* compile_let(Context& ctx, ast::LetExpression& let);
llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator);
llvm::Value* compile_unary_operator(Context& ctx, ast::UnaryOperatorExpression& unary_operator);
llvm::Value* compile_member(Context& ctx, ast::MemberExpression& member);
llvm::Value* compile_call(Context& ctx, ast::CallExpression& call);
llvm::Value* compile_cast(Context& ctx, ast::CastExpression& cast);
llvm::Value* compile_compile_time(Context& ctx, ast::CompileTimeExpression& compile_time);
llvm::Value* compile_parenthesis(Context& ctx, ast::ParenthesisExpression& parenthesis);
llvm::Value* compile_block(Context& ctx, ast::BlockExpression& block);
llvm::Function* compile_function_declaration(Context&               ctx,
                                             ast::FunctionVariable& function_variable);
llvm::Function* compile_function(Context& ctx, ast::FunctionExpression& function);
llvm::Value*    compile_if(Context& ctx, ast::IfExpression& if_);
llvm::Value*    compile_while(Context& ctx, ast::WhileExpression& while_);
llvm::Value*    compile_export(Context& ctx, ast::ExportExpression& export_);
llvm::Value*    compile_extern(Context& ctx, ast::ExternExpression& extern_);

llvm::Value* get_element_pointer(Context& ctx, ast::Expression& expression);

}  // namespace rain::lang::code
