#include "rain/lang/ast/expr/member.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

util::Result<void> MemberExpression::validate(Scope& scope) {
    if (_lhs == nullptr) {
        return ERR_PTR(err::SimpleError, "lhs is null");
    }

    {
        auto result = _lhs->validate(scope);
        FORWARD_ERROR(result);
    }

    const auto lhs_type = _lhs->type();
    if (lhs_type == nullptr) {
        return ERR_PTR(err::SimpleError, "lhs type is null");
    }
    if (lhs_type->kind() != serial::TypeKind::Struct) {
        return ERR_PTR(err::SimpleError, "lhs type is not a struct");
    }

    const auto struct_type = reinterpret_cast<ast::StructType*>(lhs_type);
    const auto member      = struct_type->find_member(_name);
    if (!member.has_value()) {
        return ERR_PTR(err::SimpleError, "member not found");
    }

    _type = struct_type->fields()[member.value()].type.get_nonnull();

    return {};
}

}  // namespace rain::lang::ast