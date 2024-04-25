{
    auto type = _f32x4_type;

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_resolved_function_type(type, binop_args, type);
    ADD_BUILTIN_METHOD(OperatorNames::Add, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Subtract, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Multiply, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Divide, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Remainder, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateFRem(arguments[0], arguments[1]); });
}
