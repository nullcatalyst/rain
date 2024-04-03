
#define ADD_BUILTIN_METHOD(name, callee_type, function_type, argument_types, impl)               \
    do {                                                                                         \
        auto method = make_builtin_function_variable(name, function_type,                        \
                                                     [](auto& ctx, auto& arguments) { impl });   \
        _function_variables.insert_or_assign(std::make_tuple(callee_type, argument_types, name), \
                                             method.get());                                      \
        _owned_variables.insert(std::move(method));                                              \
    } while (0)

{  // i32
    auto type = _i32_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_function_type(binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpNE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSGE(arguments[0], arguments[1]); });
}

{  // i64
    auto type = _i64_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_function_type(binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpNE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpSGE(arguments[0], arguments[1]); });
}

{  // f32
    auto type = _f32_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateFNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

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

    auto* cmp_type = get_function_type(binop_args, _bool_type);
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

{  // f64
    auto type = _f64_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateFNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

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

    auto* cmp_type = get_function_type(binop_args, _bool_type);
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

#undef ADD_BUILTIN_METHOD
