// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "stringformatter.h"

#include <array>

namespace openage {
namespace util {


CachableOSStream::CachableOSStream()
	:
	flag(ATOMIC_FLAG_INIT) {}


CachableOSStream::CachableOSStream(std::string &output)
	:
	CachableOSStream{} {

	this->stream.use_with(output);
}


CachableOSStream *CachableOSStream::acquire(std::string &output) {
	static std::array<CachableOSStream, 128> cache;

	// Find an available cache element.
	for (CachableOSStream &elem : cache) {
		if (elem.flag.test_and_set() == false) {
			elem.stream.use_with(output);
			return &elem;
		}
	}

	// All cached objects are currently in use... that probably means that
	// your code is broken.

	// Anyways, have fun with your dynamically-allocated object.
	// TODO Print a warning debug message about this?
	// then again, printing a log message about the logging system being
	// overloaded might not be the most clever idea.
	return new CachableOSStream{output};
}


void CachableOSStream::release(CachableOSStream *cs) {
	if (cs == nullptr) {
		return;
	}

	// std::atomic_flag doesn't have a simple way of getting the truth
	// value, so we have to do a "full" test_and_set() instead,
	// even though thread-safety is not a concern here.
	if (cs->flag.test_and_set()) {
		cs->stream.clear();
		cs->flag.clear();
	} else {
		// test_and_set returned false -> the flag was not set.
		// This is because the element was not acquired from the cache,
		// but instead dynamically allocated. de-alloc it.
		delete cs;
	}
}


}} // namespace openage::util
