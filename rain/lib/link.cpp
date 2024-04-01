#include "rain/link.hpp"

#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "rain/lang/err/simple.hpp"
#include "rain/lang/target/wasm/linker.hpp"
#include "rain/util/result.hpp"

namespace rain {

namespace {

class LlvmBuffer : public Buffer {
    std::unique_ptr<llvm::MemoryBuffer> _buffer;

  public:
    LlvmBuffer(std::unique_ptr<llvm::MemoryBuffer> buffer) : _buffer{std::move(buffer)} {}
    ~LlvmBuffer() override = default;

    std::span<const uint8_t> data() const override {
        return std::span<const uint8_t>{reinterpret_cast<const uint8_t*>(_buffer->getBufferStart()),
                                        _buffer->getBufferSize()};
    }

    std::string_view string() const override {
        return std::string_view{_buffer->getBufferStart(), _buffer->getBufferSize()};
    }
};

}  // namespace

util::Result<std::unique_ptr<Buffer>> link(lang::code::Module& module) {
    // TODO: Support more targets.
    lang::wasm::Linker linker;
    linker.add(module.llvm_module(), module.llvm_target_machine());
    auto result = linker.link();
    FORWARD_ERROR(result);
    return std::make_unique<LlvmBuffer>(std::move(result).value());
}

util::Result<std::unique_ptr<Buffer>> link(llvm::Module&        llvm_module,
                                           llvm::TargetMachine& llvm_target_machine) {
    // TODO: Support more targets.
    lang::wasm::Linker linker;
    linker.add(llvm_module, llvm_target_machine);
    auto result = linker.link();
    FORWARD_ERROR(result);
    return std::make_unique<LlvmBuffer>(std::move(result).value());
}

util::Result<std::unique_ptr<Buffer>> link(const std::string_view llvm_ir,
                                           llvm::TargetMachine&   llvm_target_machine) {
    // NOTE: Make sure that the LLVMContext lives as long as the Module.
    std::shared_ptr<llvm::LLVMContext> llvm_ctx = std::make_shared<llvm::LLVMContext>();

    llvm::SMDiagnostic            llvm_err;
    auto                          llvm_buffer = llvm::MemoryBuffer::getMemBufferCopy(llvm_ir);
    std::unique_ptr<llvm::Module> llvm_module = llvm::parseIR(*llvm_buffer, llvm_err, *llvm_ctx);
    if (llvm_module == nullptr) {
        return ERR_PTR(lang::err::SimpleError, llvm_err.getMessage().str());
    }

    return link(*llvm_module, llvm_target_machine);
}

}  // namespace rain
