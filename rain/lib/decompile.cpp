#include "rain/decompile.hpp"

#include <memory>
#include <span>
#include <vector>

#include "rain/lang/err/simple.hpp"
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

using namespace lang;

namespace {

class WabtBuffer : public Buffer {
    std::vector<uint8_t> _buffer;

  public:
    WabtBuffer(std::vector<uint8_t> buffer) : _buffer{std::move(buffer)} {}
    ~WabtBuffer() override = default;

    std::span<const uint8_t> data() const override { return _buffer; }

    std::string_view string() const override {
        return std::string_view{reinterpret_cast<const char*>(_buffer.data()),
                                reinterpret_cast<const char*>(_buffer.data()) + _buffer.size()};
    }
};

}  // namespace

util::Result<std::unique_ptr<Buffer>> decompile(const std::span<const uint8_t> wasm) {
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
    return std::make_unique<WabtBuffer>(std::move(stream.ReleaseOutputBuffer()->data));
}

}  // namespace rain
