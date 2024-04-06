#include "rain/lang/ast/expr/type.hpp"

#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

util::Result<void> TypeExpression::validate(Options& options, Scope& scope) {
    auto type_ptr = _declare_type.get_nonnull();

    switch (type_ptr->kind()) {
        case serial::TypeKind::Struct: {
            auto struct_ptr = static_cast<ast::StructType*>(type_ptr);
            if (struct_ptr->is_named()) {
                scope.add_type(struct_ptr->name_or_empty(), _declare_type.take());
                _declare_type = struct_ptr;
            }
            return {};
        }

        default:
            util::console_error("unhandled type declaration ", type_ptr->kind());
            std::abort();
    }

    util::unreachable();
}

}  // namespace rain::lang::ast
