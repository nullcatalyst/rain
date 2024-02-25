#pragma once

#include "rain/lang/module.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

class Validator {
  public:
    Validator()  = default;
    ~Validator() = default;

    util::Result<ValidatedModule> validate(ParsedModule& module);
};

}  // namespace rain::lang
