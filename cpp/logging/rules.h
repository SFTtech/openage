// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOGGING_RULES_H_
#define OPENAGE_LOGGING_RULES_H_

#include <string>

#include "defs.h"

namespace openage {
namespace logging {
namespace rules {

/**
 * abstract log rule
 *
 * log rules are used to decide a message's minimum log level, depending on its
 * MessageInfo (mostly file name and line number).
 */
class BaseRule {
public:
	const level lvl;

	BaseRule(level lvl);
	virtual ~BaseRule() = default;

	virtual bool applies(const MessageInfo &info) const = 0;
	virtual bool supersedes(const BaseRule &rule) const = 0;
	virtual std::string str() const = 0;
};

class Global: public rules::BaseRule {
public:
	Global(level lvl);

	virtual bool applies(const MessageInfo &info) const;
	virtual bool supersedes(const BaseRule &rule) const;
	virtual std::string str() const;
};

class File: public rules::BaseRule {
public:
	const std::string pattern;

	File(level lvl, const std::string pattern);

	virtual bool applies(const MessageInfo &info) const;
	virtual bool supersedes(const BaseRule &rule) const;
	virtual std::string str() const;
};

}}} // namespace openage::logging::rules

#endif
