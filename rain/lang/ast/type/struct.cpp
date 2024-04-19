#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

StructType::StructType(std::string_view name, std::vector<StructField> fields,
                       lex::Location location)
    : _name(std::move(name)), _fields(std::move(fields)), _location(location) {}

util::Result<absl::Nonnull<Type*>> StructType::_resolve(Options& options, Scope& scope) {
    for (auto& field : _fields) {
        auto resolved_type = field.type->resolve(options, scope);
        FORWARD_ERROR(resolved_type);

        field.type = std::move(resolved_type).value();
    }

    return this;
}

}  // namespace rain::lang::ast
