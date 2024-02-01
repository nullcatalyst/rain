#pragma once

#include <string_view>

#include "cirrus/code/builder.hpp"
#include "cirrus/code/linker.hpp"
#include "cirrus/lang/lexer.hpp"
#include "cirrus/lang/parser.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus {

util::Result<code::Module> compile(const std::string_view source);

}  // namespace cirrus
