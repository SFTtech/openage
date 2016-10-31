// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "coord_xy.h"

#include <cstdint>

#include "../testing/testing.h"
#include "../util/stringformatter.h"

namespace openage {
namespace coord {
namespace tests {

namespace {

// pack these test types into an anonynous namespace to prevent them from
// polluting the rest of the project.

struct TestCoordsRelative;
struct TestCoordsAbsolute;

struct TestCoordsAbsolute : CoordXYAbsolute<int, TestCoordsAbsolute, TestCoordsRelative> {
    using CoordXYAbsolute<int, TestCoordsAbsolute, TestCoordsRelative>::CoordXYAbsolute;
};

struct TestCoordsRelative : CoordXYRelative<int, TestCoordsAbsolute, TestCoordsRelative> {
    using CoordXYRelative<int, TestCoordsAbsolute, TestCoordsRelative>::CoordXYRelative;
};

} // anonymous namespace


/**
 * test method for the base CoordXY* classes.
 */
void coord() {
    // test comparision
    TESTEQUALS(TestCoordsAbsolute(3, 4) == TestCoordsAbsolute(3, 4), true);
    TESTEQUALS(TestCoordsAbsolute(3, 4) != TestCoordsAbsolute(3, 4), false);
    TESTEQUALS(TestCoordsAbsolute(3, 4) == TestCoordsAbsolute(3, 5), false);
    TESTEQUALS(TestCoordsAbsolute(3, 4) == TestCoordsAbsolute(4, 4), false);
    TESTEQUALS(TestCoordsAbsolute(3, 4) != TestCoordsAbsolute(3, 5), true);

    TESTEQUALS(TestCoordsRelative(3, 4) == TestCoordsRelative(3, 4), true);
    TESTEQUALS(TestCoordsRelative(3, 4) != TestCoordsRelative(3, 4), false);
    TESTEQUALS(TestCoordsRelative(3, 4) == TestCoordsRelative(3, 5), false);
    TESTEQUALS(TestCoordsRelative(3, 4) == TestCoordsRelative(4, 4), false);
    TESTEQUALS(TestCoordsRelative(3, 4) != TestCoordsRelative(3, 5), true);

    // test copy constructor / assignment operator
    TestCoordsAbsolute a0(1, 2);
    TestCoordsRelative r0(3, 4);

    TESTEQUALS(a0, TestCoordsAbsolute(1, 2));
    TESTEQUALS(r0, TestCoordsRelative(3, 4));

    TestCoordsAbsolute a1{a0};
    TestCoordsRelative r1{r0};

    TESTEQUALS(a0, a1);
    TESTEQUALS(r0, r1);

    a1 = a0;
    r1 = r0;

    TESTEQUALS(a0, a1);
    TESTEQUALS(r0, r1);

    // test unary +/-
    TESTEQUALS(-r0, TestCoordsRelative(-3, -4));
    TESTEQUALS(+r0, TestCoordsRelative(3, 4));

    // test addition
    TESTEQUALS(r0 + r0, TestCoordsRelative(6, 8));
    TESTEQUALS(a0 + r0, TestCoordsAbsolute(4, 6));
    TESTEQUALS(r0 + a0, TestCoordsAbsolute(4, 6));

    // test scalar multiplication and division
    TESTEQUALS(r0 * 3, TestCoordsRelative(9, 12));
    TESTEQUALS(r0 / 3, TestCoordsRelative(1, 1));

    // test string representation
    util::FString s;
    s << TestCoordsAbsolute(1, 2);
    TESTEQUALS(std::string(s), "[x: 1, y: 2]");

    s.reset();
    s << TestCoordsRelative(3, 4);
    TESTEQUALS(std::string(s), "(x: 3, y: 4)");
}

}}} // openage::coord::tests
