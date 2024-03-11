#include "rain/serial/module.hpp"

#include "rain/crypto/sha256.hpp"
#include "rain/err/simple.hpp"
#include "rain/serial/header_v0.hpp"

namespace rain::serial {

util::Result<Module> Module::from_memory(std::unique_ptr<uint8_t[]> data, const size_t size) {
    if (size < sizeof(Header)) {
        return ERR_PTR(err::SimpleError, "file too small to contain header information");
    }

    const auto* hdr = reinterpret_cast<const Header*>(data.get());
    if (hdr->magic != RAIN_MAGIC_NUMBER) {
        return ERR_PTR(err::SimpleError, "unknown file type: invalid magic number");
    }

    if (hdr->sha256 != crypto::sha256::hash(&hdr->version, size - offsetof(Header, version))) {
        return ERR_PTR(err::SimpleError, "file integrity check failed: sha256 mismatch");
    }

    if (hdr->version != 0) {
        return ERR_PTR(err::SimpleError, "unknown version number");
    }

    Module mod;
    mod._memory_size = size;
    mod._memory      = std::move(data);
    mod._set_pointers();

    if (static_cast<const void*>(&mod._memory[mod._memory_size]) <
        static_cast<const void*>(&mod._strings[mod._strings_size])) {
        return ERR_PTR(err::SimpleError,
                       "invalid file contents: file references data past the end of the file");
    }

    return mod;
}

void Module::_set_pointers() {
    const auto* hdr = reinterpret_cast<const Header*>(_memory.get());

    if (hdr->version != 0) {
        // Only version 0 is currently supported.
        return;
    }

    const auto* v0 = reinterpret_cast<const HeaderV0*>(hdr);

    _types             = reinterpret_cast<const Type*>(&v0[1]);
    _types_count       = v0->types_count;
    _expressions       = reinterpret_cast<const Expression*>(&_types[_types_count]);
    _expressions_count = v0->expressions_count;
    _indices           = reinterpret_cast<const uint32_t*>(&_expressions[_expressions_count]);
    _indices_count     = v0->indices_count;
    _strings           = reinterpret_cast<const char*>(_indices[_indices_count]);
    _strings_size      = v0->strings_size;
}

}  // namespace rain::serial
