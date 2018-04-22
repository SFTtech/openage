// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "enum_test.h"

#include "../testing/testing.h"


namespace openage {
namespace util {
namespace tests {

static constexpr testenum_value undefined {{"UNDEFINED", 0}, "undefined test string"};
testenum::testenum() : Enum{undefined} {}

void enum_() {
	testenum tv0 = testenum::foo;
	tv0 == testenum::foo or TESTFAIL;
	tv0 != testenum::bar or TESTFAIL;

	testenum tv1 = testenum::bar;
	tv0 == tv1 and TESTFAIL;

	tv1 = testenum::foo;
	tv0 == tv1 or TESTFAIL;
	tv0 != tv1 and TESTFAIL;

	TESTEQUALS(tv0->get_stuff(), std::string("foooooooooooooooooo"));
	TESTEQUALS(tv0->stuff, std::string("foooooooooooooooooo"));

	// "default value"
	testenum tv2;
	TESTEQUALS(tv2->stuff, std::string("undefined test string"));

	tv2 != tv0 or TESTFAIL;
	tv2 != tv1 or TESTFAIL;
	tv2 == tv2 or TESTFAIL;

	FString fstr;
	fstr << tv0;
	TESTEQUALS(fstr.buffer, "openage::util::tests::testenum_value::foo");

	fstr.reset();
	fstr << tv2;
	TESTEQUALS(fstr.buffer, "openage::util::tests::testenum_value::UNDEFINED");
}

}}} // openage::util::tests
