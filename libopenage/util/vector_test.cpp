// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "vector.h"

#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {

void vector() {
	{
		// tests in 2 dimensions.
		// we want to be able to reuse the variable names later.
		const Vector<2> a(1.0, 2.0);
		const Vector<2> b(3.0, 4.0);
		Vector<2> c;

		c = a + b;
		TESTEQUALS(c[0], 4.0);
		TESTEQUALS(c[1], 6.0);

		c = a - b;
		TESTEQUALS(c[0], -2.0);
		TESTEQUALS(c[1], -2.0);

		c = 5 * a;
		TESTEQUALS(c[0], 5.0);
		TESTEQUALS(c[1], 10.0);

		// division by 8 should be precise
		c = a / 8;
		TESTEQUALS(c[0], 0.125);
		TESTEQUALS(c[1], 0.25);

		TESTEQUALS(a.dot_product(b), 11);

		c = b;
		TESTEQUALS(c.norm(), 5);
		c.normalize();
		TESTEQUALS_FLOAT(c.norm(), 1, 1e-7);
	}

	{
		// test for the cross_product
		const Vector<3> a(1.0, 2.0, 3.0);
		const Vector<3> b(4.0, 5.0, 6.0);
		Vector<3> c = a.cross_product(b);
		TESTEQUALS(c[0], -3.0);
		TESTEQUALS(c[1],  6.0);
		TESTEQUALS(c[2], -3.0);
	}
}

}}} // openage::util::tests
