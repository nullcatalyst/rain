{  // u8
    auto type = _u8_type;

    auto  unop_args = Scope::TypeList{type};
    auto* unop_type = get_resolved_function_type(type, unop_args, type);
    ADD_BUILTIN_METHOD(OperatorNames::Negative, type, unop_type, unop_args,
                       { return ctx.llvm_builder().CreateNeg(arguments[0]); });
    ADD_BUILTIN_METHOD(OperatorNames::Positive, type, unop_type, unop_args,
                       { return arguments[0]; });

    auto  binop_args = Scope::TypeList{type, type};
    auto* math_type  = get_resolved_function_type(type, binop_args, type);
    ADD_BUILTIN_METHOD(OperatorNames::Add, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateAdd(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Subtract, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateSub(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Multiply, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateMul(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Divide, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateUDiv(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Remainder, type, math_type, binop_args,
                       { return ctx.llvm_builder().CreateURem(arguments[0], arguments[1]); });

    auto* cmp_type = get_resolved_function_type(type, binop_args, _bool_type);
    ADD_BUILTIN_METHOD(OperatorNames::Equal, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpEQ(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::NotEqual, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpNE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Less, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpULT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::LessEqual, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpULE(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::Greater, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpUGT(arguments[0], arguments[1]); });
    ADD_BUILTIN_METHOD(OperatorNames::GreaterEqual, type, cmp_type, binop_args,
                       { return ctx.llvm_builder().CreateICmpUGE(arguments[0], arguments[1]); });

    {
        auto  cast_args = Scope::TypeList{_i32_type};
        auto* cast_type = get_resolved_function_type(type, cast_args, type);
        ADD_BUILTIN_METHOD(OperatorNames::CastFrom, type, cast_type, cast_args, {
            auto& llvm_ir = ctx.llvm_builder();
            return llvm_ir.CreateTrunc(arguments[0], llvm_ir.getInt8Ty());
        });
    }

    {
        auto  cast_args = Scope::TypeList{_i64_type};
        auto* cast_type = get_resolved_function_type(type, cast_args, type);
        ADD_BUILTIN_METHOD(OperatorNames::CastFrom, type, cast_type, cast_args, {
            auto& llvm_ir = ctx.llvm_builder();
            return llvm_ir.CreateTrunc(arguments[0], llvm_ir.getInt8Ty());
        });
    }

    {
        auto  cast_args = Scope::TypeList{_f32_type};
        auto* cast_type = get_resolved_function_type(type, cast_args, type);
        ADD_BUILTIN_METHOD(OperatorNames::CastFrom, type, cast_type, cast_args, {
            auto& llvm_ir = ctx.llvm_builder();
            return llvm_ir.CreateFPToUI(arguments[0], llvm_ir.getInt8Ty());
        });
    }

    {
        auto  cast_args = Scope::TypeList{_f64_type};
        auto* cast_type = get_resolved_function_type(type, cast_args, type);
        ADD_BUILTIN_METHOD(OperatorNames::CastFrom, type, cast_type, cast_args, {
            auto& llvm_ir = ctx.llvm_builder();
            return llvm_ir.CreateFPToUI(arguments[0], llvm_ir.getInt8Ty());
        });
    }
}
