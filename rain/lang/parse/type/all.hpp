#pragma once

#include "absl/base/nullability.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/interface.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/type/unresolved.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/assert.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::ArrayType*>> parse_array_type(lex::Lexer& lexer, ast::Scope& scope);
util::Result<absl::Nonnull<ast::OptionalType*>>  parse_optional_type(lex::Lexer& lexer,
                                                                     ast::Scope& scope);
util::Result<absl::Nonnull<ast::StructType*>>    parse_struct_type(lex::Lexer& lexer,
                                                                   ast::Scope& scope);
util::Result<absl::Nonnull<ast::InterfaceType*>> parse_interface_type(lex::Lexer& lexer,
                                                                      ast::Scope& scope);

}  // namespace rain::lang::parse
