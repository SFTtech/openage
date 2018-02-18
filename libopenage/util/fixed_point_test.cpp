// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#include "fixed_point.h"

#include <cstdint>

#include "../testing/testing.h"
#include "stringformatter.h"

namespace openage {
namespace util {
namespace tests {


void fixed_point() {
	// test construction and assignments
	FixedPoint<int8_t, 4> a(4.3);
	TESTEQUALS(a.to_int(), 4);
	TESTEQUALS(a.to_float(), 4.25);
	TESTEQUALS(a.to_double(), 4.25);
	TESTEQUALS(a.get_raw_value(), 68);
	TESTEQUALS(a.get_fractional_part().to_float(), 0.25);

	FixedPoint<int8_t, 4> b(a);
	TESTEQUALS(b.get_raw_value(), 68);

	auto c = FixedPoint<uint16_t, 8>::from_fixedpoint<int8_t, 4>(a);
	TESTEQUALS(c.get_raw_value(), 1088);
	TESTEQUALS(c.to_int(), 4);
	TESTEQUALS(c.to_double(), 4.25);

	a = FixedPoint<int8_t, 4>::from_int(1);
	TESTEQUALS(a.get_raw_value(), 16);

	a = FixedPoint<int8_t, 4>::from_float(1.3);
	TESTEQUALS(a.get_raw_value(), 20);

	a = FixedPoint<int8_t, 4>::from_double(1.4);
	TESTEQUALS(a.get_raw_value(), 22);

	auto d = FixedPoint<uint64_t, 48>::from_double(2016);
	// this raw value was calculated in python: 2016 * 2**48
	TESTEQUALS(d.get_raw_value(), 567453553048682496UL);

	// test arithmetics
	using TestType = FixedPoint<int64_t, 32>;

	TestType e(120.7);
	TestType f(-12.4);
	e += f;

	TESTEQUALS_FLOAT(e.to_double(), 108.3, 1e-7);
	TESTEQUALS_FLOAT((-e).to_double(), -108.3, 1e-7);
	TESTEQUALS_FLOAT((+e).to_double(), 108.3, 1e-7);
	TESTEQUALS_FLOAT((e - f).to_double(), 120.7, 1e-7);
	TESTEQUALS_FLOAT((e + f).to_double(), 95.9, 1e-7);
	TESTEQUALS_FLOAT((e * 1.02).to_double(), 108.3 * 1.02, 1e-7);
	TESTEQUALS_FLOAT((e * 10).to_double(), 108.3 * 10, 1e-7);
	TESTEQUALS_FLOAT((e / 10).to_double(), 108.3 / 10, 1e-7);
	TESTEQUALS_FLOAT(std::sqrt(e), sqrt(108.3), 1e-7);
	TESTEQUALS_FLOAT(std::abs(-e).to_double(), 108.3, 1e-7);
	TESTEQUALS_FLOAT(std::hypot(e, f), hypot(108.3, -12.4), 1e-7);
	TESTEQUALS_FLOAT(std::min(e, f), -12.4, 1e-7);
	TESTEQUALS_FLOAT(std::max(e, f), 108.3, 1e-7);
	TESTEQUALS_FLOAT(TestType::min_value().to_double(), -2147483648.0, 1e-7);
	TESTEQUALS_FLOAT(TestType::max_value().to_double(), 2147483648.0, 1e-7);

	e -= f;

	TESTEQUALS_FLOAT(e.to_double(), 120.7, 1e-7);

	TESTEQUALS(e == f, false);
	TESTEQUALS(e != f, true);
	TESTEQUALS(e == e, true);
	TESTEQUALS(e <= e, true);
	TESTEQUALS(e >= e, true);
	TESTEQUALS(e >= f, true);
	TESTEQUALS(e <= f, false);
	TESTEQUALS(e < e, false);
	TESTEQUALS(e > e, false);
	TESTEQUALS(e < f, false);
	TESTEQUALS(e > f, true);

	// test the string I/O functions
	FString s;
	s << a;
	TESTEQUALS(std::string(s), "1.38");

	s.reset();
	s << c;
	TESTEQUALS(std::string(s), "4.250");

	s.reset();
	s << d;
	TESTEQUALS(std::string(s), "2016.000000000000000");

	std::stringstream sstr("1234.5678");
	sstr >> e;
	TESTEQUALS_FLOAT(e.to_double(), 1234.5678, 1e-7);
}

}}} // openage::util::tests
