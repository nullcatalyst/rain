#include "rain/lang/options.hpp"

namespace rain::lang {

bool Options::extern_is_compile_time_runnable(const std::span<const std::string> keys) {
    return false;
}

void Options::compile_extern_compile_time_runnable(code::Context&                     ctx,
                                                   llvm::Function*                    llvm_function,
                                                   const std::span<const std::string> keys) {}

}  // namespace rain::lang
