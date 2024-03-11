#include "rain/serial/builder.hpp"

#include "rain/serial/header_v0.hpp"

namespace rain::serial {

Module Builder::build() {
    const auto types_size       = sizeof(Type) * _types.size();
    const auto expressions_size = sizeof(Expression) * _expressions.size();
    const auto variables_size   = sizeof(Variable) * _variables.size();
    const auto indices_size     = sizeof(uint32_t) * _indices.size();
    const auto strings_size     = sizeof(char) * _strings.size();

    const auto memory_size = sizeof(HeaderV0) + types_size + expressions_size + variables_size +
                             indices_size + strings_size;

    Module mod;
    mod._memory      = std::make_unique<uint8_t[]>(memory_size);
    mod._memory_size = memory_size;

    auto* ptr = mod._memory.get();
    // For sanity, zero out the memory.
    std::memset(ptr, 0, memory_size);

    auto* v0        = reinterpret_cast<HeaderV0*>(ptr);
    v0->hdr.magic   = RAIN_MAGIC_NUMBER;
    v0->hdr.version = 0;

    v0->types_count       = _types.size();
    v0->expressions_count = _expressions.size();
    v0->variables_count   = _variables.size();
    v0->indices_count     = _indices.size();
    v0->strings_size      = _strings.size();

    mod._set_pointers();

    std::memcpy(const_cast<Type*>(mod._types), _types.data(), types_size);
    std::memcpy(const_cast<Expression*>(mod._expressions), _expressions.data(), expressions_size);
    std::memcpy(const_cast<Variable*>(mod._variables), _variables.data(), variables_size);
    std::memcpy(const_cast<uint32_t*>(mod._indices), _indices.data(), indices_size);
    std::memcpy(const_cast<char*>(mod._strings), _strings.data(), strings_size);

    v0->hdr.sha256 =
        crypto::sha256::hash(&v0->hdr.version, memory_size - offsetof(Header, version));

    return mod;
}

}  // namespace rain::serial