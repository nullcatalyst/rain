#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

StructType::StructType(std::string_view name, std::vector<StructField> fields,
                       lex::Location location)
    : _name(std::move(name)), _fields(std::move(fields)), _location(location) {
    for (auto& field : _fields) {
        field.type->add_ref(*this);
    }
}

util::Result<absl::Nonnull<Type*>> StructType::resolve(Options& options, Scope& scope) {
    for (auto& field : _fields) {
        auto resolved_type = field.type->resolve(options, scope);
        FORWARD_ERROR(resolved_type);

        field.type = std::move(resolved_type).value();
    }

    return this;
}

void StructType::replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) {
    for (auto& field : _fields) {
        if (field.type == old_type) {
            field.type = new_type;
        }
    }

    IF_DEBUG {
        // Only remove the ref in debug/test builds, so that a (potentially) expensive search isn't
        // needed for every replaced instance of every type.
        old_type->remove_ref(*this);
    }
}

}  // namespace rain::lang::ast
