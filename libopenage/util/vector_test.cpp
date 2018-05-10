// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "vector.h"

#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {

void vector() {
	{
		// zero-initialization test.
		Vector<5> zero_explicit{0, 0, 0, 0, 0};
		Vector<5> zero;

		zero.equals(zero_explicit) or TESTFAIL;
	}
	{
		// tests in 2 dimensions.
		// we want to be able to reuse the variable names later.
		const Vector<2> a(1.0, 2.0);
		const Vector<2> b(3.0, 4.0);
		Vector<2> c;

		// test basic operators.
		c = a + b;
		c.equals({4.0, 6.0}) or TESTFAIL;

		c = a - b;
		c.equals({-2.0, -2.0}) or TESTFAIL;

		c = 5 * a;
		c.equals({5.0, 10.0}) or TESTFAIL;

		c = a / 8;
		c.equals({0.125, 0.25}) or TESTFAIL;

		c.equals({13, 37}) and TESTFAIL;

		// test dot product, norm and normalization.
		TESTEQUALS_FLOAT(a.dot(b), 11, 1e-7);

		c = b;
		TESTEQUALS_FLOAT(c.norm(), 5, 1e-7);

		c.normalize();
		TESTEQUALS_FLOAT(c.norm(), 1, 1e-7);
	}

	{
		// test for the cross_product
		const Vector<3> a(1.0, 2.0, 3.0);
		const Vector<3> b(4.0, 5.0, 6.0);
		Vector<3> c = a.cross_product(b);

		c.equals({-3.0, 6.0, -3.0}) or TESTFAIL;
	}
}

}}} // openage::util::tests
