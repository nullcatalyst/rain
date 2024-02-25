#include "rain/lang/validator.hpp"

#include "rain/err/all.hpp"

namespace rain::lang {

util::Result<ValidatedModule> Validator::validate(ParsedModule& module) {
    // TODO: Implement validation
    return OK(ValidatedModule, std::move(module.nodes()));
}

}  // namespace rain::lang
