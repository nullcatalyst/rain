#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> StructType::resolve(Options& options, Scope& scope) {
    for (auto& field : _fields) {
        auto resolved_type = field.type->resolve(options, scope);
        FORWARD_ERROR(resolved_type);

        field.type = std::move(resolved_type).value();
    }

    return this;
}

}  // namespace rain::lang::ast