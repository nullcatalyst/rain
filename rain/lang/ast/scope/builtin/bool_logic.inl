{  // bool
    auto type = _bool_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_resolved_function_type(type, unop_args, type);
    ADD_BUILTIN_METHOD(OperatorNames::Not, type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateNot(arguments[0]); });

    auto  binop_args = Scope::TypeList{type, type};
    auto* logic_type = get_resolved_function_type(type, binop_args, type);
    ADD_BUILTIN_METHOD(OperatorNames::And, type, logic_type, binop_args,
                       { return ctx.llvm_builder().CreateAnd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Or, type, logic_type, binop_args,
                       { return ctx.llvm_builder().CreateOr(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Xor, type, logic_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpNE(arguments[0], arguments[1]); });

    auto* cmp_type = get_resolved_function_type(type, binop_args, type);
    ADD_BUILTIN_METHOD(OperatorNames::Equal, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::NotEqual, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpNE(arguments[0], arguments[1]); });
}
