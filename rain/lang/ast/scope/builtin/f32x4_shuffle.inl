{
    auto type = _f32x4_type;

    auto  unop_args = Scope::TypeList{type};
    auto* math_type = get_resolved_function_type(type, unop_args, type);

#define SHUF_0 "x"
#define SHUF_1 "y"
#define SHUF_2 "z"
#define SHUF_3 "w"

#define SHUFFLE_MASK_4(a, b, c, d)                                                        \
    ADD_BUILTIN_METHOD(SHUF_##a SHUF_##b SHUF_##c SHUF_##d, type, math_type, unop_args, { \
        const std::array<int, 4> shuffle_mask{a, b, c, d};                                \
        return ctx.llvm_builder().CreateShuffleVector(                                    \
            arguments[0], llvm::UndefValue::get(arguments[0]->getType()), shuffle_mask);  \
    })

#define SHUFFLE_MASK_3(a, b, c) \
    SHUFFLE_MASK_4(a, b, c, 0); \
    SHUFFLE_MASK_4(a, b, c, 1); \
    SHUFFLE_MASK_4(a, b, c, 2); \
    SHUFFLE_MASK_4(a, b, c, 3)

#define SHUFFLE_MASK_2(a, b) \
    SHUFFLE_MASK_3(a, b, 0); \
    SHUFFLE_MASK_3(a, b, 1); \
    SHUFFLE_MASK_3(a, b, 2); \
    SHUFFLE_MASK_3(a, b, 3)

#define SHUFFLE_MASK_1(a) \
    SHUFFLE_MASK_2(a, 0); \
    SHUFFLE_MASK_2(a, 1); \
    SHUFFLE_MASK_2(a, 2); \
    SHUFFLE_MASK_2(a, 3)

#define CREATE_ALL_SHUFFLE_MASKS()
    SHUFFLE_MASK_1(0);
    SHUFFLE_MASK_1(1);
    SHUFFLE_MASK_1(2);
    SHUFFLE_MASK_1(3)

    CREATE_ALL_SHUFFLE_MASKS();

#undef CREATE_ALL_SHUFFLE_MASKS
#undef SHUFFLE_MASK_1
#undef SHUFFLE_MASK_2
#undef SHUFFLE_MASK_3
#undef SHUFFLE_MASK_4
#undef SHUF_0
#undef SHUF_1
#undef SHUF_2
#undef SHUF_3
}
