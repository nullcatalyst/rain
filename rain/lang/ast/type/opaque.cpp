#include "rain/lang/ast/type/opaque.hpp"

#include "rain/util/console.hpp"

namespace rain::lang::ast {

OpaqueType::OpaqueType(std::string_view name) : _name(std::move(name)) { _resolves_to = this; }

util::Result<absl::Nonnull<Type*>> OpaqueType::_resolve(Options& options, Scope& scope) {
    return this;
}

}  // namespace rain::lang::ast
