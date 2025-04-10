// Copyright 2016-2025 the openage authors. See copying.md for legal info.

#include "fixed_point.h"

#include <cstdint>

#include "../testing/testing.h"
#include "stringformatter.h"
#include "math_constants.h"

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
	TESTEQUALS_FLOAT(std::atan2(e, f), atan2(108.3, -12.4), 1e-7);
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

	TESTEQUALS_FLOAT(TestType::e().to_double(), math::E, 1e-7);
	TESTEQUALS_FLOAT(TestType::log2e().to_double(), math::LOG2E, 1e-7);
	TESTEQUALS_FLOAT(TestType::log10e().to_double(), math::LOG10E, 1e-7);
	TESTEQUALS_FLOAT(TestType::ln2().to_double(), math::LN2, 1e-7);
	TESTEQUALS_FLOAT(TestType::ln10().to_double(), math::LN10, 1e-7);
	TESTEQUALS_FLOAT(TestType::pi().to_double(), math::PI, 1e-7);
	TESTEQUALS_FLOAT(TestType::pi_2().to_double(), math::PI_2, 1e-7);
	TESTEQUALS_FLOAT(TestType::pi_4().to_double(), math::PI_4, 1e-7);
	TESTEQUALS_FLOAT(TestType::inv_pi().to_double(), math::INV_PI, 1e-7);
	TESTEQUALS_FLOAT(TestType::inv2_pi().to_double(), math::INV2_PI, 1e-7);
	TESTEQUALS_FLOAT(TestType::inv2_sqrt_pi().to_double(), math::INV2_SQRT_PI, 1e-7);
	TESTEQUALS_FLOAT(TestType::tau().to_double(), math::TAU, 1e-7);
	TESTEQUALS_FLOAT(TestType::degs_per_rad().to_double(), math::DEGSPERRAD, 1e-7);
	TESTEQUALS_FLOAT(TestType::rads_per_deg().to_double(), math::RADSPERDEG, 1e-7);
	TESTEQUALS_FLOAT(TestType::sqrt_2().to_double(), math::SQRT_2, 1e-7);
	TESTEQUALS_FLOAT(TestType::inv_sqrt_2().to_double(), math::INV_SQRT_2, 1e-7);


	using TestTypeShort = FixedPoint<int32_t, 16>;
	TESTEQUALS_FLOAT(TestTypeShort::e().to_double(), math::E, 1e-3);
	TESTEQUALS_FLOAT(TestTypeShort::pi().to_double(), math::PI, 1e-3);

	{
		using S = FixedPoint<uint16_t, 7U>;
		using T = FixedPoint<uint16_t, 7U, uint64_t>;

		auto a = S::from_int(16U);
		TESTNOTEQUALS((a*a).to_int(), 256U);

		auto b = T::from_int(16U);
		TESTEQUALS((b*b).to_int(), 256U);

		auto c = T::from_int(17U);
		TESTEQUALS((c*c).to_int(), 289U);
	}
	{
		using S = FixedPoint<int32_t, 12U>;
		auto a = S::from_int(256);
		auto b = S::from_int(8);
		TESTNOTEQUALS((a/b).to_int(), 32);


		using T = FixedPoint<int32_t, 12, int64_t>;
		auto c = T::from_int(256);
		auto d = T::from_int(8);
		TESTEQUALS((c/d).to_int(), 32);
	}
	{
		using T = FixedPoint<int32_t, 12, int64_t>;
		auto a = T::from_double(4.75);
		auto b = T::from_double(3.5);
		auto c = -a;
		TESTEQUALS_FLOAT((a/b).to_double(), 4.75/3.5, 0.1);
		TESTEQUALS_FLOAT((c/b).to_double(), -4.75/3.5, 0.1);
	}

}

}}} // openage::util::tests
