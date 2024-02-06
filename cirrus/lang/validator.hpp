#pragma once

#include "cirrus/lang/module.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::lang {

class Validator {
  public:
    Validator()  = default;
    ~Validator() = default;

    util::Result<ValidatedModule> validate(ParsedModule& module);
};

}  // namespace cirrus::lang
