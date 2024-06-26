#include <string>

#include "rain/bin/common.hpp"
#include "rain/lang/target/wasm/options.hpp"
#include "rain/rain.hpp"
#include "rain/util/console.hpp"

namespace {

rain::lang::wasm::Options _options;

}  // namespace

WASM_EXPORT("init")
void initialize() {
#if defined(__wasm__)
    __wasm_call_ctors();
#endif  // defined(__wasm__)

    rain::lang::wasm::initialize_llvm();
}

WASM_EXPORT("set_memory_export")
void export_memory(const char* memory_name_start, const char* memory_name_end) {
    _options.set_memory_export_name(std::string{memory_name_start, memory_name_end});
}

WASM_EXPORT("set_stack_size")
void set_stack_size(uint32_t stack_size) { _options.set_stack_size(stack_size); }

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end, bool optimize) {
    using namespace rain;

    static std::string prev_result;
    prev_result.clear();

    // Compile the source code.
    auto compile_result = rain::compile(std::string_view{source_start, source_end}, _options);
    if (!compile_result.has_value()) {
        const auto msg = compile_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }
    auto rain_module = std::move(compile_result).value();

    if (optimize) {
        rain_module.optimize();
    }

    // Get the LLVM IR.
    auto ir_result = rain_module.emit_ir();
    if (!ir_result.has_value()) {
        const auto msg = ir_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }
    auto ir = std::move(ir_result).value();
    callback(rain::Action::CompileRain, ir.c_str(), ir.c_str() + ir.size());

    // Link the module into WebAssembly.
    auto link_result = rain::link(rain_module, _options);
    if (!link_result.has_value()) {
        const auto msg = link_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }
    auto wasm = std::move(link_result).value();
    callback(rain::Action::CompileLLVM, &*wasm->string().begin(), &*wasm->string().end());

    // Decompile the WebAssembly into WAT.
    auto decompile_result = rain::decompile(wasm->data());
    if (!decompile_result.has_value()) {
        const auto msg = decompile_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }
    auto wat = std::move(decompile_result).value();
    callback(rain::Action::DecompileWasm, &*wat->string().begin(), &*wat->string().end());
}

#if !defined(__wasm__)

#include <fstream>

int main(const int argc, const char* const argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <wasm file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::string source;
    file.seekg(0, std::ios::end);
    source.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(source.data(), source.size());

    rain::util::console_log(ANSI_CYAN, "Source code:\n", ANSI_RESET, source, "\n");

    initialize();

#define ABORT_ON_ERROR(result, msg)                        \
    if (!result.has_value()) {                             \
        rain::util::panic(msg, result.error()->message()); \
    }

    auto compile_result = rain::compile(source, _options);
    ABORT_ON_ERROR(compile_result, "Failed to compile: ");
    auto rain_mod = std::move(compile_result).value();

    rain_mod.optimize();

    auto ir = rain_mod.emit_ir();
    ABORT_ON_ERROR(ir, "Failed to emit IR: ");
    auto ir_bytes = std::move(ir).value();
    rain::util::console_log(ANSI_CYAN, "LLVM_IR:\n", ANSI_RESET, ir_bytes, "\n");

    auto wasm_result = rain::link(rain_mod, _options);
    ABORT_ON_ERROR(wasm_result, "Failed to link: ");
    auto wasm_bytes = std::move(wasm_result).value();

    auto wat_result = rain::decompile(wasm_bytes->data());
    ABORT_ON_ERROR(wat_result, "Failed to decompile: ");
    auto wat_bytes = std::move(wat_result).value();

    rain::util::console_log(ANSI_CYAN, "WAT:\n", ANSI_RESET, wat_bytes->string(), "\n");

#undef ABORT_ON_ERROR
}

#endif  // !defined(__wasm__)
