// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "rules.h"

#include "logging.h"
#include "../util/strings.h"

namespace openage {
namespace logging {
namespace rules {

BaseRule::BaseRule(level lvl)
	:
	lvl{lvl} {}

Global::Global(level lvl)
	:
	BaseRule{lvl} {}

bool Global::applies(const MessageInfo &/* info */) const {
	return true;
}

bool Global::supersedes(const BaseRule &/* rule */) const {
	return true;
}

std::string Global::str() const {
	return "global rule";
}

File::File(level lvl, const std::string pattern)
	:
	BaseRule{lvl},
	pattern{pattern} {}

bool File::applies(const MessageInfo &info) const {
	return util::string_matches_pattern(info.sourcefile, this->pattern.c_str());
}

bool File::supersedes(const BaseRule &rule) const {
	{
		const Global *casted = dynamic_cast<const Global *>(&rule);
		if (casted) {
			return util::string_matches_pattern("*", this->pattern.c_str());
		}
	}

	{
		const File *casted = dynamic_cast<const File *>(&rule);
		if (casted) {
			return util::string_matches_pattern(casted->pattern.c_str(), this->pattern.c_str());
		}
	}

	// unknown type of rule

	// TODO log some sort of error message... without risking to trigger an infinite loop of some sorts.
	// logging errors in the logger is hard...

	return false;
}

std::string File::str() const {
	// TODO
	return "file rule...";
}

}}} // namespace openage::logging::rules
