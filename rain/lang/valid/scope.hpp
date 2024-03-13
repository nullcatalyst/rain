#pragma once

#include "absl/containers/flat_hash_map.h"

namespace rain::lang::valid {

class Scope {
  public:
    virtual ~Scope() = default;
};

class BuiltinScope : public Scope {
    std::shared_ptr<TypePtr> _types;

    ast::TypePtr _i32_type;
    ast::TypePtr _i64_type;
    ast::TypePtr _f32_type;
    ast::TypePtr _f64_type;

  public:
    BuiltinScope()           = default;
    ~BuiltinScope() override = default;
};

class ModuleScope : public Scope {
    absl::flat_hash_map<std::string, std::shared_ptr<Function>> _functions;

  public:
    ModuleScope()  = default;
    ~ModuleScope() = default;
};

class BlockScope {
    absl::flat_hash_map<std::string, std::shared_ptr<Variable>> _variables;

  public:
    Scope()  = default;
    ~Scope() = default;
};

}  // namespace rain::lang::valid
