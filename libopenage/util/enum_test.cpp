// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "enum_test.h"

#include "../testing/testing.h"


namespace openage {
namespace util {


template<>
tests::testtype::data_type tests::testtype::data{};


namespace tests {

namespace testtypes {

testtype foo({"some text"});
testtype bar({"some other text"});

} // testtypes


void enum_() {
	testtype tv0 = testtypes::foo;
	tv0 == testtypes::foo or TESTFAIL;
	tv0 != testtypes::bar or TESTFAIL;

	testtype tv1 = testtypes::bar;
	tv0 == tv1 and TESTFAIL;

	// "default value"
	testtype tv2;

	// the enum doesn't have 12 members; thus, this defaults to the default value.
	testtype tv3;
	tv2 == tv3 or TESTFAIL;

	FString fstr;
	fstr << tv0;
	TESTEQUALS(fstr.buffer, "openage::util::tests::testtypes::foo");

	fstr.reset();
	fstr << tv2;
	TESTEQUALS(fstr.buffer, "<default enum value>");

	TESTEQUALS(tv0.get()->stuff, "some text");
	TESTEQUALS(tv0->stuff, "some text");
	TESTEQUALS(tv2->stuff, "");
}


}}} // openage::util::tests
