#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <string>

#include "absl/container/flat_hash_map.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/node.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/lang/module.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::transform {

class Codegen {
    llvm::LLVMContext _ctx;
    llvm::Module      _mod;
    llvm::IRBuilder<> _builder;

    absl::flat_hash_map<std::string, llvm::Type*>     _types;
    absl::flat_hash_map<std::string, llvm::Function*> _functions;

  public:
    Codegen();
    virtual ~Codegen() = default;

    util::Result<void> generate(const lang::Module& mod);

    std::string llvm_ir() const;

  private:
    util::Result<llvm::Type*>       find_llvm_type(const ast::Type& type);
    util::Result<llvm::Function*>   generate(const ast::FunctionExpression& function_expression);
    util::Result<llvm::StructType*> generate(const ast::StructType& struct_type);
};

}  // namespace cirrus::transform
