// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "testing.h"

namespace openage {
namespace testing {


bool fail(const log::message &msg) {
	throw TestError(msg);
}


}} // opeange::testing
