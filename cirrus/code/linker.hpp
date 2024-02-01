#pragma once

#include "cirrus/code/module.hpp"
#include "lld/Common/CommonLinkerContext.h"
#include "lld/Common/Memory.h"
#include "lld/wasm/InputChunks.h"
#include "lld/wasm/InputElement.h"
#include "llvm/Support/MemoryBuffer.h"

namespace cirrus::code {

/**
 * This is mostly a copy of the implementation of the lld LinkerDriver class.
 *
 * Unfortunately that class requires going through the terminal interface, so all inputs and outputs
 * are files, which simply does not work in a wasm environment, so this tweaked copy is required.
 */
class Linker {
    uint32_t                 _stack_size = 0;
    std::vector<std::string> _force_export_symbols;

    std::vector<std::unique_ptr<llvm::MemoryBuffer>> _files;

  public:
    void set_stack_size(const uint32_t stack_size) noexcept { _stack_size = stack_size; }
    void force_export_symbol(const char* function_name) noexcept {
        _force_export_symbols.emplace_back(function_name);
    }

    // `memory_buffer` must be bitcode or wasm.
    void add(std::unique_ptr<llvm::MemoryBuffer> memory_buffer) {
        _files.emplace_back(std::move(memory_buffer));
    }
    util::Result<void> add(Module& mod);

    [[nodiscard]] util::Result<std::unique_ptr<llvm::MemoryBuffer>> link();
};

}  // namespace cirrus::code