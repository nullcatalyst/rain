#pragma once

#include "llvm/IR/Type.h"
#include "rain/lang/code/context.hpp"

namespace rain::lang::ast {

class ArrayType;
class FunctionType;
class InterfaceType;
class OptionalType;
class StructType;
class Type;

}  // namespace rain::lang::ast

namespace rain::lang::code {

/**
 * Fetch the already compiled type or compile it.
 *
 * Use this function whenever you need to get a reference to an LLVM type, so that type compilation
 * work isn't duplicated.
 */
llvm::Type* get_or_compile_type(Context& ctx, ast::Type& type);

/**
 * Force a new compilation of a type.
 *
 * This is primarily used to delegate to the correct type compilation function.
 */
llvm::Type* compile_type(Context& ctx, ast::Type& type);

////////////////////////////////////////////////////////////////
// Specific type compilation functions.
llvm::Type* compile_function_type(Context& ctx, ast::FunctionType& function_type);
llvm::Type* compile_struct_type(Context& ctx, ast::StructType& struct_type);
llvm::Type* compile_interface_type(Context& ctx, ast::InterfaceType& interface_type);
llvm::Type* compile_array_type(Context& ctx, ast::ArrayType& array_type);
llvm::Type* compile_optional_type(Context& ctx, ast::OptionalType& optional_type);

}  // namespace rain::lang::code
