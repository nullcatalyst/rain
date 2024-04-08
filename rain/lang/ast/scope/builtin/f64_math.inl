{  // f64
    auto type = _f64_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_resolved_function_type(type, unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateFNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_resolved_function_type(type, binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_resolved_function_type(type, binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateFCmpOEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateFCmpONE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateFCmpOLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateFCmpOLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateFCmpOGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateFCmpOGE(arguments[0], arguments[1]); });
}
