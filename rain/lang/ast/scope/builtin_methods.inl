
#define ADD_BUILTIN_METHOD(name, callee_type, function_type, argument_types, impl)                 \
    do {                                                                                           \
        auto method = make_builtin_function_variable(name, function_type,                          \
                                                     [](auto& builder, auto& arguments) { impl }); \
        _function_variables.insert_or_assign(std::make_tuple(callee_type, argument_types, name),   \
                                             method.get());                                        \
        _owned_variables.insert(std::move(method));                                                \
    } while (0)

{  // i32
    auto type = _i32_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return builder.CreateNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return builder.CreateAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return builder.CreateSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return builder.CreateMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return builder.CreateSDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return builder.CreateSRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_function_type(binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return builder.CreateICmpEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return builder.CreateICmpNE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSGE(arguments[0], arguments[1]); });
}

{  // i64
    auto type = _i64_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return builder.CreateNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return builder.CreateAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return builder.CreateSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return builder.CreateMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return builder.CreateSDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return builder.CreateSRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_function_type(binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return builder.CreateICmpEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return builder.CreateICmpNE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return builder.CreateICmpSGE(arguments[0], arguments[1]); });
}

{  // f32
    auto type = _f32_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return builder.CreateFNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return builder.CreateFAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return builder.CreateFSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return builder.CreateFMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return builder.CreateFDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return builder.CreateFRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_function_type(binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpONE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOGE(arguments[0], arguments[1]); });
}

{  // f64
    auto type = _f64_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_function_type(unop_args, type);
    ADD_BUILTIN_METHOD("__neg__", type, unop_type, unop_args,
                       { return builder.CreateFNeg(arguments[0]); });
    ADD_BUILTIN_METHOD("__pos__", type, unop_type, unop_args, { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return builder.CreateFAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return builder.CreateFSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return builder.CreateFMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return builder.CreateFDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return builder.CreateFRem(arguments[0], arguments[1]); });

    auto* cmp_type = get_function_type(binop_args, _bool_type);
    ADD_BUILTIN_METHOD("__eq__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ne__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpONE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__lt__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOLT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__le__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOLE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__gt__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__ge__", type, cmp_type, binop_args,
                       { return builder.CreateFCmpOGE(arguments[0], arguments[1]); });
}

{
    auto type = _f32x4_type;

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_function_type(binop_args, type);
    ADD_BUILTIN_METHOD("__add__", type, math_type, binop_args,
                       { return builder.CreateFAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__sub__", type, math_type, binop_args,
                       { return builder.CreateFSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__mul__", type, math_type, binop_args,
                       { return builder.CreateFMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__div__", type, math_type, binop_args,
                       { return builder.CreateFDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD("__rem__", type, math_type, binop_args,
                       { return builder.CreateFRem(arguments[0], arguments[1]); });
}

#undef ADD_BUILTIN_METHOD
