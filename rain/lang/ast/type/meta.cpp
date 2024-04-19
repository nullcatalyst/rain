#include "rain/lang/ast/type/meta.hpp"

#include "rain/util/console.hpp"

namespace rain::lang::ast {

MetaType::MetaType(absl::Nonnull<Type*> type) : _type(type) { _resolves_to = this; }

util::Result<absl::Nonnull<Type*>> MetaType::_resolve(Options& options, Scope& scope) {
    return this;
}

}  // namespace rain::lang::ast
