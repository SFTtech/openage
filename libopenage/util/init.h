// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_INIT_H_
#define OPENAGE_UTIL_INIT_H_

#include <functional>

namespace openage {
namespace util {


/**
 * Runs code() on construction.
 *
 * Designed for use as a global object.
 * Note that your regular dynamic initialization order concerns apply.
 */
class OnInit {
public:
	explicit OnInit(std::function<void ()> code) {
		code();
	}

	~OnInit() = default;

private:
	// nope.
	OnInit(const OnInit &) = delete;
	OnInit(OnInit &&) = delete;

	OnInit &operator =(const OnInit &) = delete;
	OnInit &operator =(OnInit &&) = delete;
};


/**
 * Runs code() on destruction.
 *
 * Designed mostly for use as a global object, though theoretically
 * it might be useful as a stack-allocated object in some circumstances.
 */
class OnDeInit {
public:
	explicit OnDeInit(std::function<void ()> code)
		:
		code{code} {}


	~OnDeInit() {
		this->code();
	}

private:
	std::function<void ()> code;

	// nope.
	OnDeInit(const OnDeInit &) = delete;
	OnDeInit(OnDeInit &&) = delete;

	OnDeInit &operator =(const OnDeInit &) = delete;
	OnDeInit &operator =(OnDeInit &&) = delete;
};


}} // openage::util

#endif
