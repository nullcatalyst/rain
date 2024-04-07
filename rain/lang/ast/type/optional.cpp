#include "rain/lang/ast/type/optional.hpp"

namespace rain::lang::ast {

void OptionalType::replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) {
    if (_type.get() == old_type.get()) {
        _type = new_type;
    }

    IF_DEBUG {
        // Only remove the ref in debug/test builds, so that a (potentially) expensive search isn't
        // needed for every replaced instance of every type.
        old_type->remove_ref(*this);
    }
}

}  // namespace rain::lang::ast
