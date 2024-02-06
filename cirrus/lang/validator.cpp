#include "cirrus/lang/validator.hpp"

#include "cirrus/err/all.hpp"

namespace cirrus::lang {

util::Result<ValidatedModule> Validator::validate(ParsedModule& module) {
    // TODO: Implement validation
    return OK(ValidatedModule, std::move(module.nodes()));
}

}  // namespace cirrus::lang
