#include "rain/util/wasm.hpp"
#include "wabt/apply-names.h"
#include "wabt/binary-reader-ir.h"
#include "wabt/binary-reader.h"
#include "wabt/error-formatter.h"
#include "wabt/feature.h"
#include "wabt/generate-names.h"
#include "wabt/ir.h"
#include "wabt/option-parser.h"
#include "wabt/stream.h"
#include "wabt/validator.h"
#include "wabt/wast-lexer.h"
#include "wabt/wat-writer.h"

WASM_IMPORT("env", "throw")
void throw_error(const char* msg_start, const char* msg_end);

inline void throw_error(const std::string_view msg) { throw_error(msg.begin(), msg.end()); }

WASM_IMPORT("env", "callback")
void decompile_callback(const char* buffer_start, const char* buffer_end);

WASM_EXPORT("malloc")
void* memory_allocate(size_t size) { return malloc(size); }

WASM_EXPORT("free")
void memory_free(void* ptr) { return free(ptr); }

#if defined(__wasm__)
extern "C" void __wasm_call_ctors();
#endif  // defined(__wasm__)

WASM_EXPORT("init")
void initialize() {
#if defined(__wasm__)
    __wasm_call_ctors();
#endif  // defined(__wasm__)
}

WASM_EXPORT("decompile")
void decompile(const char* source_start, const char* source_end) {
    static std::unique_ptr<wabt::OutputBuffer> prev_result;
    prev_result.reset();

    wabt::Features features;

    constexpr bool          read_debug_names             = true;
    constexpr bool          stop_on_first_error          = true;
    constexpr bool          fail_on_custom_section_error = true;
    wabt::ReadBinaryOptions options(features, nullptr, read_debug_names, stop_on_first_error,
                                    fail_on_custom_section_error);

    wabt::Errors errors;
    wabt::Module wasm_module;
    if (const auto result =
            wabt::ReadBinaryIr("<wasm>", reinterpret_cast<const uint8_t*>(source_start),
                               source_end - source_start, options, &errors, &wasm_module);
        !wabt::Succeeded(result)) {
        wabt::Color color(nullptr);
        std::string error_message =
            FormatErrorsToString(errors, wabt::Location::Type::Binary, nullptr, color);
        throw_error(error_message);
    }

    if (const auto result = wabt::GenerateNames(&wasm_module); !Succeeded(result)) {
        throw_error("Failed to generate names");
    }

    [[maybe_unused]] auto _result = wabt::ApplyNames(&wasm_module);

    wabt::WriteWatOptions wat_options(features);
    wat_options.fold_exprs    = false;
    wat_options.inline_import = false;
    wat_options.inline_export = false;

    wabt::MemoryStream stream(std::make_unique<wabt::OutputBuffer>());
    _result     = wabt::WriteWat(&stream, &wasm_module, wat_options);
    prev_result = stream.ReleaseOutputBuffer();

    decompile_callback(
        reinterpret_cast<const char*>(prev_result->data.data()),
        reinterpret_cast<const char*>(prev_result->data.data() + prev_result->data.size()));
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {}

#endif  // !defined(__wasm__)
