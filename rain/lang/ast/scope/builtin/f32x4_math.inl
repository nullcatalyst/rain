{
    auto type = _f32x4_type;

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
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
}
