#pragma once

#include <string_view>

#include "rain/lang/ast/module.hpp"
#include "rain/lang/code/module.hpp"
#include "rain/lang/options.hpp"
#include "rain/util/result.hpp"

namespace rain {

util::Result<lang::code::Module> compile(const std::string_view source);
util::Result<lang::code::Module> compile(const std::string_view source, lang::Options& options);

}  // namespace rain
