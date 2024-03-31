#include "rain/lang/ast/expr/member.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> MemberExpression::validate(Scope& scope) {
    assert(_lhs != nullptr && "lhs is null");

    {
        auto result = _lhs->validate(scope);
        FORWARD_ERROR(result);
    }

    const auto lhs_type = _lhs->type();
    if (lhs_type == nullptr) {
        return ERR_PTR(err::SyntaxError, _lhs->location(),
                       "the null type does not have any members");
    }
    if (lhs_type->kind() != serial::TypeKind::Struct) {
        return ERR_PTR(
            err::SyntaxError, _lhs->location(),
            absl::StrCat("accessing member of value that is not a struct, the value has type ",
                         lhs_type->name()));
    }

    const auto struct_type = reinterpret_cast<ast::StructType*>(lhs_type);
    const auto member      = struct_type->find_member(_name);
    if (!member.has_value()) {
        return ERR_PTR(err::SyntaxError, _member_location,
                       absl::StrCat("struct member ", _name, " not found"));
    }

    _type = struct_type->fields()[member.value()].type.get_nonnull();
    return {};
}

}  // namespace rain::lang::ast