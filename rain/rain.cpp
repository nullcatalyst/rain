#include "rain/rain.hpp"

#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "rain/code/compiler.hpp"
#include "rain/code/linker.hpp"
#include "rain/err/simple.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"
#include "rain/util/result.hpp"
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

namespace rain {

util::Result<rain::code::Module> compile(const std::string_view source) {
    lang::Lexer  lexer{source};
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    FORWARD_ERROR(parse_result);

    code::Compiler compiler;
    return compiler.build(std::move(parse_result).value());
}

util::Result<rain::code::Module> compile(const std::string_view                    source,
                                         llvm::function_ref<void(code::Compiler&)> init_compiler) {
    lang::Lexer  lexer{source};
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    FORWARD_ERROR(parse_result);

    code::Compiler compiler;
    init_compiler(compiler);
    return compiler.build(std::move(parse_result).value());
}

util::Result<std::unique_ptr<llvm::MemoryBuffer>> link(rain::code::Module& module) {
    rain::code::Linker linker;
    linker.add(module);
    return linker.link();
}

util::Result<std::unique_ptr<llvm::MemoryBuffer>> link(llvm::Module& module) {
    rain::code::Linker linker;
    linker.add(module);
    return linker.link();
}

util::Result<std::unique_ptr<llvm::MemoryBuffer>> link(const std::string_view llvm_ir) {
    // NOTE: Make sure that the LLVMContext lives as long as the Module.
    std::shared_ptr<llvm::LLVMContext> llvm_ctx = std::make_shared<llvm::LLVMContext>();

    llvm::SMDiagnostic            llvm_err;
    auto                          llvm_buffer = llvm::MemoryBuffer::getMemBufferCopy(llvm_ir);
    std::unique_ptr<llvm::Module> llvm_mod    = llvm::parseIR(*llvm_buffer, llvm_err, *llvm_ctx);
    if (llvm_mod == nullptr) {
        return ERR_PTR(err::SimpleError, llvm_err.getMessage().str());
    }

    rain::code::Linker linker;
    linker.add(*llvm_mod);
    return linker.link();
}

util::Result<std::unique_ptr<wabt::OutputBuffer>> decompile(const std::span<const uint8_t> wasm) {
    wabt::Features features;

    constexpr bool          read_debug_names             = true;
    constexpr bool          stop_on_first_error          = true;
    constexpr bool          fail_on_custom_section_error = true;
    wabt::ReadBinaryOptions options(features, nullptr, read_debug_names, stop_on_first_error,
                                    fail_on_custom_section_error);

    wabt::Errors errors;
    wabt::Module wasm_module;
    if (const auto result =
            wabt::ReadBinaryIr("<wasm>", reinterpret_cast<const uint8_t*>(wasm.data()),
                               wasm.size_bytes(), options, &errors, &wasm_module);
        !wabt::Succeeded(result)) {
        wabt::Color color(nullptr);
        std::string error_message =
            FormatErrorsToString(errors, wabt::Location::Type::Binary, nullptr, color);
        return ERR_PTR(err::SimpleError, error_message);
    }

    if (const auto result = wabt::GenerateNames(&wasm_module); !Succeeded(result)) {
        return ERR_PTR(err::SimpleError, "failed to generate names");
    }

    [[maybe_unused]] auto _result = wabt::ApplyNames(&wasm_module);

    wabt::WriteWatOptions wat_options(features);
    wat_options.fold_exprs    = false;
    wat_options.inline_import = false;
    wat_options.inline_export = false;

    wabt::MemoryStream stream(std::make_unique<wabt::OutputBuffer>());
    _result = wabt::WriteWat(&stream, &wasm_module, wat_options);
    return stream.ReleaseOutputBuffer();
}

}  // namespace rain
