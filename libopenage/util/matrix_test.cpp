// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "matrix.h"
#include "vector.h"

#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {

void matrix() {
	{ // matrix multiplication
		const Matrix<5, 3> a(0.0, 0.5, 1.0,
		                     1.5, 2.0, 2.5,
		                     3.0, 3.5, 4.0,
		                     4.5, 5.0, 5.5,
		                     6.0, 6.5, 7.0);
		const Matrix<3, 4> b(1, 0, 0, 0,
		                     0, 1, 0, 0,
		                     0, 0, 1, 0);
		Matrix<5, 4> c = a * b;
		TESTEQUALS_FLOAT(c[0][0], 0, 1e-7);
		TESTEQUALS_FLOAT(c[0][1], 0.5, 1e-7);
		TESTEQUALS_FLOAT(c[0][2], 1, 1e-7);
		TESTEQUALS_FLOAT(c[0][3], 0, 1e-7);
		TESTEQUALS_FLOAT(c[1][0], 1.5, 1e-7);
		TESTEQUALS_FLOAT(c[1][1], 2, 1e-7);
		TESTEQUALS_FLOAT(c[1][2], 2.5, 1e-7);
		TESTEQUALS_FLOAT(c[1][3], 0, 1e-7);
		TESTEQUALS_FLOAT(c[2][0], 3, 1e-7);
		TESTEQUALS_FLOAT(c[2][1], 3.5, 1e-7);
		TESTEQUALS_FLOAT(c[2][2], 4, 1e-7);
		TESTEQUALS_FLOAT(c[2][3], 0, 1e-7);
		TESTEQUALS_FLOAT(c[3][0], 4.5, 1e-7);
		TESTEQUALS_FLOAT(c[3][1], 5, 1e-7);
		TESTEQUALS_FLOAT(c[3][2], 5.5, 1e-7);
		TESTEQUALS_FLOAT(c[3][3], 0, 1e-7);
		TESTEQUALS_FLOAT(c[4][0], 6, 1e-7);
		TESTEQUALS_FLOAT(c[4][1], 6.5, 1e-7);
		TESTEQUALS_FLOAT(c[4][2], 7, 1e-7);
		TESTEQUALS_FLOAT(c[4][3], 0, 1e-7);
	}

	{ // addition and subtraction
		const Matrix2 a(1, 2, 3, 4);
		const Matrix2 b(5, 6, 7, 8);
		Matrix2 c = a + b;
		TESTEQUALS_FLOAT(c[0][0], 6, 1e-7);
		TESTEQUALS_FLOAT(c[0][1], 8, 1e-7);
		TESTEQUALS_FLOAT(c[1][0], 10, 1e-7);
		TESTEQUALS_FLOAT(c[1][1], 12, 1e-7);

		c = b - a;
		TESTEQUALS_FLOAT(c[0][0], 4, 1e-7);
		TESTEQUALS_FLOAT(c[0][1], 4, 1e-7);
		TESTEQUALS_FLOAT(c[1][0], 4, 1e-7);
		TESTEQUALS_FLOAT(c[1][1], 4, 1e-7);
	}

	{ // scalar multiplication and division
		const Matrix2 a(0, 1, 2, 3);
		Matrix2 b = a * 2;
		TESTEQUALS_FLOAT(b[0][0], 0, 1e-7);
		TESTEQUALS_FLOAT(b[0][1], 2, 1e-7);
		TESTEQUALS_FLOAT(b[1][0], 4, 1e-7);
		TESTEQUALS_FLOAT(b[1][1], 6, 1e-7);

		b = 2 * a;
		TESTEQUALS_FLOAT(b[0][0], 0, 1e-7);
		TESTEQUALS_FLOAT(b[0][1], 2, 1e-7);
		TESTEQUALS_FLOAT(b[1][0], 4, 1e-7);
		TESTEQUALS_FLOAT(b[1][1], 6, 1e-7);

		b = a / 0.5;
		TESTEQUALS_FLOAT(b[0][0], 0, 1e-7);
		TESTEQUALS_FLOAT(b[0][1], 2, 1e-7);
		TESTEQUALS_FLOAT(b[1][0], 4, 1e-7);
		TESTEQUALS_FLOAT(b[1][1], 6, 1e-7);
	}

	{ // transposition
		const Matrix2 a(1, 2, 3, 4);
		const Matrix2 b = a.transpose();
		TESTEQUALS_FLOAT(b[0][0], 1, 1e-7);
		TESTEQUALS_FLOAT(b[0][1], 3, 1e-7);
		TESTEQUALS_FLOAT(b[1][0], 2, 1e-7);
		TESTEQUALS_FLOAT(b[1][1], 4, 1e-7);
	}

	{ // vector interaction
		const Matrix3 a(2, 0, 0,
		                0, 2, 0,
		                0, 0, 2);
		const Vector3 v(1, 2, 3);
		auto u = (a * v).to_vector();
		TESTEQUALS_FLOAT(u[0], 2, 1e-7);
		TESTEQUALS_FLOAT(u[1], 4, 1e-7);
		TESTEQUALS_FLOAT(u[2], 6, 1e-7);
	}
}

}}} // openage::util::tests
