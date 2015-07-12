// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "constinit_vector.h"

#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {


// no linkage for this test type
namespace {


struct S {
	int val;
};


class T {
public:
	explicit T(int *ref)
		:
		ref{ref} {

		*(this->ref) += 1;
	}

	~T() {
		if (this->ref) {
			*(this->ref) -= 1;
		}
	}

	T(const T &other) : ref{other.ref} {
		(*this->ref) += 1;
	}

	T &operator =(const T &other) {
		this->ref = other.ref;
		(*this->ref) += 1;
		return *this;
	}

private:
	int *ref;
};


} // anonymous namespace



void constinit_vector() {
	ConstInitVector<S> sv;

	for (int i = 0; i < 1337; i++) {
		sv.push_back(S{i});
	}

	sv.size() == 1337 or TESTFAIL;
	sv[235].val == 235 or TESTFAIL;

	int refctr = 0;

	{ // scope to test whether tv gets correctly de-inited.

	ConstInitVector<T> tv;
	for (int i = 0; i < 1337; i++) {
		tv.push_back(T(&refctr));
	}

	refctr == 1337 or TESTFAIL;
	sv.size() == 1337 or TESTFAIL;

	}

	refctr == 0 or TESTFAIL;
}


}}} // openage::util::tests
