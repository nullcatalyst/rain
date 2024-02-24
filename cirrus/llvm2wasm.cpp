#include <string_view>

#include "cirrus/code/initialize.hpp"
#include "cirrus/code/linker.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/log.hpp"
#include "cirrus/util/wasm.hpp"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"

WASM_IMPORT("env", "error")
void throw_error(const char* msg_start, const char* msg_end);

WASM_IMPORT("env", "callback")
void compile_callback(const char* buffer_start, const char* buffer_end);

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

    cirrus::code::initialize_llvm();
    // cirrus::code::Compiler::use_external_function("sqrt", lle_X_sqrt);
}

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::unique_ptr<llvm::MemoryBuffer> prev_result;
    prev_result.reset();

    // NOTE: Make sure that the LLVMContext lives as long as the Module.
    std::shared_ptr<llvm::LLVMContext> llvm_ctx = std::make_shared<llvm::LLVMContext>();
    std::unique_ptr<llvm::Module>      llvm_mod;

    std::string_view source{source_start, source_end};
    cirrus::util::console_log(ANSI_CYAN, "Source code:\n", ANSI_RESET, source, "\n");

    {
        llvm::SMDiagnostic llvm_err;
        auto               llvm_buffer = llvm::MemoryBuffer::getMemBufferCopy(source);
        llvm_mod                       = llvm::parseIR(*llvm_buffer, llvm_err, *llvm_ctx);
        if (llvm_mod == nullptr) {
            cirrus::util::console_error(ANSI_RED, "failed to parse IR: ", ANSI_RESET,
                                        llvm_err.getMessage().str());
            std::abort();
        }
    }
    {
        cirrus::code::Linker linker;
        linker.add(*llvm_mod);

        auto result = linker.link();
        if (!result.has_value()) {
            cirrus::util::console_error(ANSI_RED, "failed to link: ", ANSI_RESET,
                                        result.error()->message());
            std::abort();
        }
        prev_result = std::move(result).value();
    }

    if (prev_result == nullptr) {
        cirrus::util::console_error(ANSI_RED, "failed to link: ", ANSI_RESET, "result is null");
        std::abort();
    }

    compile_callback(prev_result->getBufferStart(), prev_result->getBufferEnd());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {
    const std::string source = R"(
; ModuleID = 'cirrus'
source_filename = "cirrus"
target datalayout = "e-m:e-p:32:32-p10:8:8-p20:8:8-i64:64-n32:64-S128-ni:1:10:20"
target triple = "wasm32-unknown-unknown"

; Function Attrs: nofree nosync nounwind memory(none)
define internal fastcc i32 @fib(i32 %n) unnamed_addr #0 {
entry:
  %0 = icmp slt i32 %n, 2
  br i1 %0, label %common.ret, label %merge

common.ret:                                       ; preds = %merge, %entry
  %accumulator.tr.lcssa = phi i32 [ 0, %entry ], [ %4, %merge ]
  %n.tr.lcssa = phi i32 [ %n, %entry ], [ %3, %merge ]
  %accumulator.ret.tr = add i32 %n.tr.lcssa, %accumulator.tr.lcssa
  ret i32 %accumulator.ret.tr

merge:                                            ; preds = %entry, %merge
  %n.tr2 = phi i32 [ %3, %merge ], [ %n, %entry ]
  %accumulator.tr1 = phi i32 [ %4, %merge ], [ 0, %entry ]
  %1 = add nsw i32 %n.tr2, -1
  %2 = tail call fastcc i32 @fib(i32 %1)
  %3 = add nsw i32 %n.tr2, -2
  %4 = add i32 %2, %accumulator.tr1
  %5 = icmp ult i32 %n.tr2, 4
  br i1 %5, label %common.ret, label %merge
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define dso_local i32 @double(i32 %n) local_unnamed_addr #1 {
entry:
  %0 = shl i32 %n, 1
  ret i32 %0
}

; Function Attrs: nofree nosync nounwind memory(none)
define dso_local i32 @run() local_unnamed_addr #2 {
entry:
  %0 = tail call fastcc i32 @fib(i32 6)
  ret i32 %0
}

attributes #0 = { nofree nosync nounwind memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "wasm-export-name"="double" }
attributes #2 = { nofree nosync nounwind memory(none) "wasm-export-name"="run" }
)";
    cirrus::util::console_log(ANSI_CYAN, "LLVM IR:\n", ANSI_RESET, source, '\n');

    initialize();
    auto wasm_buffer = compile(&*source.cbegin(), &*source.cend());

    return 0;
}

#endif  // !defined(__wasm__)

#undef ABORT_ON_ERROR
