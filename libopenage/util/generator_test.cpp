// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include <iostream>
#include <vector>

#include "generator.h"
#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {

class GenInc : public Generator<int> {
private:
	int num = 1337;
protected:
	virtual void generate() {
		this->yield(this->num++);
	}
};

void generator() {
	GenInc gen;
	for (int i = 1337; i < 9002; i++) {
		TESTEQUALS(gen(), i);
	}
}

}}} // openage::util::tests
