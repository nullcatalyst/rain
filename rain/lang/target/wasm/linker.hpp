#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "lld/Common/CommonLinkerContext.h"
#include "lld/Common/Memory.h"
#include "lld/wasm/InputChunks.h"
#include "lld/wasm/InputElement.h"
#include "llvm/Support/MemoryBuffer.h"
#include "rain/util/result.hpp"

namespace rain::lang::wasm {

/**
 * This is mostly a copy of the implementation of the lld LinkerDriver class.
 *
 * Unfortunately that class requires going through the terminal interface, so all inputs and outputs
 * are files, which simply does not work in a wasm environment, so this tweaked copy is required.
 */
class Linker {
    std::optional<uint32_t>  _stack_size;
    std::string_view         _memory_export_name;
    std::vector<std::string> _force_export_symbols;

    std::vector<std::unique_ptr<llvm::MemoryBuffer>> _files;

  public:
    void set_stack_size(const uint32_t stack_size) noexcept { _stack_size = stack_size; }
    void set_memory_export_name(std::string_view memory_export_name) noexcept {
        _memory_export_name = memory_export_name;
    }
    void force_export_symbol(const char* function_name) noexcept {
        _force_export_symbols.emplace_back(function_name);
    }

    // `memory_buffer` must be bitcode or wasm.
    void add(std::unique_ptr<llvm::MemoryBuffer> memory_buffer) {
        _files.emplace_back(std::move(memory_buffer));
    }
    util::Result<void> add(llvm::Module& llvm_module, llvm::TargetMachine& llvm_target_machine);

    [[nodiscard]] util::Result<std::unique_ptr<llvm::MemoryBuffer>> link();
};

}  // namespace rain::lang::wasm
