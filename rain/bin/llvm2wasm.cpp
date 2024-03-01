#include "rain/bin/common.hpp"
#include "rain/code/initialize.hpp"

#define RAIN_INCLUDE_LINK
#include "rain/rain.hpp"

namespace rain {

WASM_EXPORT("init")
void initialize() { code::initialize_llvm(); }

}  // namespace rain

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::unique_ptr<rain::Buffer> prev_result;
    prev_result.reset();

    // Link the LLVM IR into WebAssembly.
    auto link_result = rain::link(std::string_view{source_start, source_end});
    if (!link_result.has_value()) {
        rain::throw_error(link_result.error()->message());
    }

    prev_result = std::move(link_result).value();
    callback(rain::Action::CompileLLVM, prev_result->string().begin(), prev_result->string().end());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {
    const std::string source = R"(
; ModuleID = 'rain'
source_filename = "rain"
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
    rain::util::console_log(ANSI_CYAN, "LLVM IR:\n", ANSI_RESET, source, "\n");

    initialize();
    compile(&*source.cbegin(), &*source.cend());
    return 0;
}

#endif  // !defined(__wasm__)
