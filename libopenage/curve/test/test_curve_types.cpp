// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../../testing/testing.h"
#include "../internal/keyframe_container.h"
#include "../curve.h"
#include "../continuous.h"
#include "../discrete.h"


namespace openage {
namespace curve {
namespace tests {

void curve_types() {
	// Check the base container type
	EventQueue q;
	TriggerFactory f{&q};
	{
		KeyframeContainer<int> c;
		auto p0 = c.insert(0, 0);
		auto p1 = c.insert(1, 1);
		auto p2 = c.insert(10, 2);

		// last function tests without hints
		TESTEQUALS(c.last(0)->value, 0);
		TESTEQUALS(c.last(1)->value, 1); //last shall give >= not only > !
		TESTEQUALS(c.last(5)->value, 1);
		TESTEQUALS(c.last(10)->value, 2);
		TESTEQUALS(c.last(47)->value, 2);

		// last() with hints. Yes this can make a difference. we want to be
		// absolutely shure!
		// hint p1
		TESTEQUALS(c.last(0, p0)->value, 0);
		TESTEQUALS(c.last(1, p0)->value, 1); //last shall give >= not only > !
		TESTEQUALS(c.last(5, p0)->value, 1);
		TESTEQUALS(c.last(10, p0)->value, 2);
		TESTEQUALS(c.last(47, p0)->value, 2);

		TESTEQUALS(c.last(0, p1)->value, 0);
		TESTEQUALS(c.last(1, p1)->value, 1); //last shall give >= not only > !
		TESTEQUALS(c.last(5, p1)->value, 1);
		TESTEQUALS(c.last(10, p1)->value, 2);
		TESTEQUALS(c.last(47, p1)->value, 2);

		TESTEQUALS(c.last(0, p2)->value, 0);
		TESTEQUALS(c.last(1, p2)->value, 1); //last shall give >= not only > !
		TESTEQUALS(c.last(5, p2)->value, 1);
		TESTEQUALS(c.last(10, p2)->value, 2);
		TESTEQUALS(c.last(47, p2)->value, 2);

		// Now test the basic erase() function
		c.erase(c.last(1));

		TESTEQUALS(c.last(1)->value, 0);
		TESTEQUALS(c.last(5)->value, 0);
		TESTEQUALS(c.last(47)->value, 2);

		c.erase_after(c.last(99)); // we dont want to have the element itself erased!
		TESTEQUALS(c.last(47)->value, 2);

		c.erase_after(c.last(5)); // now since 10 > 5, element with value 2 has to be gone
		TESTEQUALS(c.last(47)->value, 0);
	}

	// Check the Simple Continuous type
	{
		Continuous<float> c(&f);
		c.set_insert(0, 0);
		c.set_insert(10, 1);

		TESTEQUALS(c.get(0), 0);

		TESTEQUALS_FLOAT(c.get(1), 0.1, 1e-7);
	}

	{
		Continuous<float> c(&f);
		c.set_insert(0, 0);
		c.set_insert(20, 20);

		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 1);
		TESTEQUALS(c.get(7), 7);

		c.set_drop(20, 10);
		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(2), 1);
		TESTEQUALS(c.get(8), 4);
	}

	//Check the discrete type
	{
		Discrete<int> c(&f);
		c.set_insert(0, 0);
		c.set_insert(10, 10);

		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 0);
		TESTEQUALS(c.get(10), 10);

		Discrete<std::string> complex(&f);

		complex.set_insert(0, "Test 0");
		complex.set_insert(10, "Test 10");

		TESTEQUALS(complex.get(0), "Test 0");
		TESTEQUALS(complex.get(1), "Test 0");
		TESTEQUALS(complex.get(10), "Test 10");

	}

	//check set_drop
	{
		Discrete<int> c(&f);
		c.set_insert(0, 0);
		c.set_insert(1, 1);
		c.set_insert(3, 3);

		TESTEQUALS(c.get(3), 3);

		c.set_drop(2, 10);
		TESTEQUALS(c.get(2), 10);
	}

	// Encountered Errors
	{
		Continuous<int> c(&f);
		c.set_insert(1, 1);
		c.set_drop(1, -5);

		TESTEQUALS(c.get(1), -5);
	}
}

}}} // openage::curve::tests
