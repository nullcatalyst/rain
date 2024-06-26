#include "rain/lang/ast/expr/member.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

bool MemberExpression::is_assignable() const noexcept {
    if (_lhs->type()->kind() == serial::TypeKind::Reference) {
        return true;
    }

    return _lhs->is_assignable();
}

util::Result<void> MemberExpression::validate(Options& options, Scope& scope) {
    assert(_lhs != nullptr && "lhs is null");

    {
        auto result = _lhs->validate(options, scope);
        FORWARD_ERROR(result);
    }

    auto* lhs_type = _lhs->type();
    if (lhs_type == nullptr) {
        return ERR_PTR(err::SyntaxError, _lhs->location(),
                       "the null type does not have any members");
    }

    if (lhs_type->kind() == serial::TypeKind::Reference) {
        lhs_type = &static_cast<ast::ReferenceType*>(lhs_type)->type();
    }

    if (lhs_type->kind() != serial::TypeKind::Struct) {
        return ERR_PTR(
            err::SyntaxError, _lhs->location(),
            absl::StrCat("accessing member of value that is not a struct, the value has type ",
                         lhs_type->display_name()));
    }

    const auto struct_type = reinterpret_cast<ast::StructType*>(lhs_type);
    const auto member      = struct_type->find_member(_name);
    if (!member.has_value()) {
        return ERR_PTR(err::SyntaxError, _member_location,
                       absl::StrCat("struct member ", _name, " not found"));
    }

    _type = struct_type->fields()[member.value()].type;
    return {};
}

}  // namespace rain::lang::ast
