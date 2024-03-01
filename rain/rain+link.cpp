#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "rain/code/linker.hpp"
#include "rain/err/simple.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"
#include "rain/util/result.hpp"

#define RAIN_INCLUDE_LINK
#include "rain/rain.hpp"

namespace rain {

namespace {

class LlvmBuffer : public Buffer {
    std::unique_ptr<llvm::MemoryBuffer> _buffer;

  public:
    LlvmBuffer(std::unique_ptr<llvm::MemoryBuffer> buffer) : _buffer{std::move(buffer)} {}
    ~LlvmBuffer() override = default;

    std::span<const uint8_t> data() const {
        return std::span<const uint8_t>{reinterpret_cast<const uint8_t*>(_buffer->getBufferStart()),
                                        _buffer->getBufferSize()};
    }

    std::string_view string() const {
        return std::string_view{_buffer->getBufferStart(), _buffer->getBufferSize()};
    }
};

}  // namespace

util::Result<std::unique_ptr<Buffer>> link(rain::code::Module& module) {
    rain::code::Linker linker;
    linker.add(module);
    auto result = linker.link();
    FORWARD_ERROR(result);
    return std::make_unique<LlvmBuffer>(std::move(result).value());
}

util::Result<std::unique_ptr<Buffer>> link(llvm::Module& module) {
    rain::code::Linker linker;
    linker.add(module);
    auto result = linker.link();
    FORWARD_ERROR(result);
    return std::make_unique<LlvmBuffer>(std::move(result).value());
}

util::Result<std::unique_ptr<Buffer>> link(const std::string_view llvm_ir) {
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
    auto result = linker.link();
    FORWARD_ERROR(result);
    return std::make_unique<LlvmBuffer>(std::move(result).value());
}

}  // namespace rain
