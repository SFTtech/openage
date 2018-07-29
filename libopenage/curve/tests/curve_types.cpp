// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "../../event/loop.h"
#include "../../log/log.h"
#include "../../testing/testing.h"
#include "../continuous.h"
#include "../curve.h"
#include "../discrete.h"
#include "../keyframe_container.h"


namespace openage::curve::tests {

void curve_types() {
	// Check the base container type
	{
		auto loop = std::make_shared<event::Loop>();
		KeyframeContainer<int> c{loop};

		auto p0 = c.insert(0, 0);
		auto p1 = c.insert(1, 1);
		auto p2 = c.insert(10, 2);

		// last function tests without hints
		TESTEQUALS(c.last(0)->value, 0);
		TESTEQUALS(c.last(1)->value, 1); // last shall give >= not only > !
		TESTEQUALS(c.last(5)->value, 1);
		TESTEQUALS(c.last(10)->value, 2);
		TESTEQUALS(c.last(47)->value, 2);

		// last() with hints. Yes this can make a difference. we want to be
		// absolutely sure!
		TESTEQUALS(c.last(0, p0)->value, 0);
		TESTEQUALS(c.last(1, p0)->value, 1); // last shall give >= not only > !
		TESTEQUALS(c.last(5, p0)->value, 1);
		TESTEQUALS(c.last(10, p0)->value, 2);
		TESTEQUALS(c.last(47, p0)->value, 2);

		TESTEQUALS(c.last(0, p1)->value, 0);
		TESTEQUALS(c.last(1, p1)->value, 1); // last shall give >= not only > !
		TESTEQUALS(c.last(5, p1)->value, 1);
		TESTEQUALS(c.last(10, p1)->value, 2);
		TESTEQUALS(c.last(47, p1)->value, 2);

		TESTEQUALS(c.last(0, p2)->value, 0);
		TESTEQUALS(c.last(1, p2)->value, 1); // last shall give >= not only > !
		TESTEQUALS(c.last(5, p2)->value, 1);
		TESTEQUALS(c.last(10, p2)->value, 2);
		TESTEQUALS(c.last(47, p2)->value, 2);

		// Now test the basic erase() function
		// Delete the 1-element, new values should be [0, 2]
		c.erase(c.last(1));

		TESTEQUALS(c.last(1)->value, 0);
		TESTEQUALS(c.last(5)->value, 0);
		TESTEQUALS(c.last(47)->value, 2);

		// should do nothing, since we delete all at > 99,
		// but the last element is at 10.
		c.erase_after(c.last(99));
		TESTEQUALS(c.last(47)->value, 2);

		// now since 5 < 10, element with value 2 has to be gone
		c.erase_after(c.last(5));
		TESTEQUALS(c.last(47)->value, 0);
	}

	// Check the Simple Continuous type
	{
		auto f = std::make_shared<event::Loop>();
		Continuous<float> c(f, 0);

		c.set_insert(0, 0);
		c.set_insert(10, 1);

		TESTEQUALS(c.get(0), 0);

		TESTEQUALS_FLOAT(c.get(1), 0.1, 1e-7);
	}

	{
		auto f = std::make_shared<event::Loop>();
		Continuous<float> c(f, 0);
		log::log(DBG << "Testing Continuous float");
		c.set_insert(0, 0);
		log::log(DBG << "Inserting t:20, v:20");
		c.set_insert(20, 20);

		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 1);
		TESTEQUALS(c.get(7), 7);

		c.set_last(20, 10);
		log::log(DBG << "Inserting t:20, v:10. This should overwrite the old values!");
		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(2), 1);
		TESTEQUALS(c.get(8), 4);
	}

	//Check the discrete type
	{
		auto f = std::make_shared<event::Loop>();
		Discrete<int> c(f, 0);
		c.set_insert(0, 0);
		c.set_insert(10, 10);

		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 0);
		TESTEQUALS(c.get(10), 10);

		Discrete<std::string> complex(f, 0);

		complex.set_insert(0, "Test 0");
		complex.set_insert(10, "Test 10");

		TESTEQUALS(complex.get(0), "Test 0");
		TESTEQUALS(complex.get(1), "Test 0");
		TESTEQUALS(complex.get(10), "Test 10");

	}

	//check set_last
	{
		auto f = std::make_shared<event::Loop>();
		Discrete<int> c(f, 0);
		c.set_insert(0, 0);
		c.set_insert(1, 1);
		c.set_insert(3, 3);

		TESTEQUALS(c.get(3), 3);

		c.set_last(2, 10);
		TESTEQUALS(c.get(2), 10);
	}

	// Encountered Errors
	{
		auto f = std::make_shared<event::Loop>();
		Continuous<int> c(f, 0);
		c.set_insert(1, 1);
		c.set_last(1, -5);

		TESTEQUALS(c.get(1), -5);
	}
}

} // openage::curve::tests
