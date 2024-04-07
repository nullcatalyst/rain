#include "rain/lang/ast/type/unresolved.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/simple.hpp"
#include "rain/util/console.hpp"

namespace rain::lang::ast {

void UnresolvedType::add_ref(Expression& expression) noexcept {
    _expression_refs.emplace_back(&expression);
}

void UnresolvedType::remove_ref(Expression& expression) noexcept {
    _expression_refs.erase(
        std::remove(_expression_refs.begin(), _expression_refs.end(), &expression),
        _expression_refs.end());
}

void UnresolvedType::add_ref(Type& type) noexcept { _type_refs.emplace_back(&type); }

void UnresolvedType::remove_ref(Type& type) noexcept {
    _type_refs.erase(std::remove(_type_refs.begin(), _type_refs.end(), &type), _type_refs.end());
}

util::Result<absl::Nonnull<Type*>> UnresolvedType::resolve(Options& options, Scope& scope) {
    auto resolved_type = scope.find_type(_name);
    if (resolved_type == nullptr) {
        // TODO: Add a location to the error
        return ERR_PTR(err::SimpleError, absl::StrCat("no type \"", _name, "\" found in scope"));
    }

    return resolved_type;
}

util::Result<void> UnresolvedType::resolve_to_type(absl::Nonnull<Type*> resolved_type) {
    for (auto& type : _type_refs) {
        type->replace_type(this, resolved_type);
    }

    for (auto& expression : _expression_refs) {
        expression->replace_type(this, resolved_type);
    }

    IF_DEBUG {
        if (_expression_refs.size() > 0) {
            util::panic("attempted to resolve the unresolved type \"", _name,
                        "\", but there are still ", _expression_refs.size(),
                        " expressions referencing this type; this is an internal error");
        }

        if (_type_refs.size() > 0) {
            util::panic("attempted to resolve the unresolved type \"", _name,
                        "\", but there are still ", _type_refs.size(),
                        " types referencing this type; this is an internal error");
        }
    }

    _expression_refs.clear();
    _type_refs.clear();

    return {};
}

}  // namespace rain::lang::ast
