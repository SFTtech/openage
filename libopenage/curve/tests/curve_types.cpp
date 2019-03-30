// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "../../event/loop.h"
#include "../../log/log.h"
#include "../../testing/testing.h"
#include "../continuous.h"
#include "../curve.h"
#include "../discrete.h"
#include "../keyframe_container.h"
#include "../segmented.h"

#include "../../util/compiler.h"

namespace openage::curve::tests {

void curve_types() {
	// Check the base container type
	{
		auto loop = std::make_shared<event::Loop>();
		KeyframeContainer<int> c;

		auto p0 = c.insert_before(0, 0);
		auto p1 = c.insert_before(1, 1);
		auto p2 = c.insert_before(10, 2);
		auto pa = std::begin(c);
		auto pe = std::end(c);

		// now contains: [-inf: 0, 0:0, 1:1, 10:2]

		TESTEQUALS(c.size(), 4);

		{
			auto it = c.begin();
			TESTEQUALS(it->value, 0);
			TESTEQUALS(it->time, std::numeric_limits<curve::time_t>::min());
			TESTEQUALS((++it)->time, 0);
			TESTEQUALS(it->value, 0);
			TESTEQUALS((++it)->time, 1);
			TESTEQUALS(it->value, 1);
			TESTEQUALS((++it)->time, 10);
			TESTEQUALS(it->value, 2);
		}

		// last function tests without hints
		TESTEQUALS(c.last(0)->value, 0);
		TESTEQUALS(c.last(1)->value, 1);
		TESTEQUALS(c.last(5)->value, 1);
		TESTEQUALS(c.last(10)->value, 2);
		TESTEQUALS(c.last(47)->value, 2);

		// last() with hints.
		TESTEQUALS(c.last(0, pa)->value, 0);
		TESTEQUALS(c.last(1, pa)->value, 1);
		TESTEQUALS(c.last(5, pa)->value, 1);
		TESTEQUALS(c.last(10, pa)->value, 2);
		TESTEQUALS(c.last(47, pa)->value, 2);

		TESTEQUALS(c.last(0, p0)->value, 0);
		TESTEQUALS(c.last(1, p0)->value, 1);
		TESTEQUALS(c.last(5, p0)->value, 1);
		TESTEQUALS(c.last(10, p0)->value, 2);
		TESTEQUALS(c.last(47, p0)->value, 2);

		TESTEQUALS(c.last(0, p1)->value, 0);
		TESTEQUALS(c.last(1, p1)->value, 1);
		TESTEQUALS(c.last(5, p1)->value, 1);
		TESTEQUALS(c.last(10, p1)->value, 2);
		TESTEQUALS(c.last(47, p1)->value, 2);

		TESTEQUALS(c.last(0, p2)->value, 0);
		TESTEQUALS(c.last(1, p2)->value, 1);
		TESTEQUALS(c.last(5, p2)->value, 1);
		TESTEQUALS(c.last(10, p2)->value, 2);
		TESTEQUALS(c.last(47, p2)->value, 2);

		TESTEQUALS(c.last(0, pe)->value, 0);
		TESTEQUALS(c.last(1, pe)->value, 1);
		TESTEQUALS(c.last(5, pe)->value, 1);
		TESTEQUALS(c.last(10, pe)->value, 2);
		TESTEQUALS(c.last(47, pe)->value, 2);

		// Now test the basic erase() function
		// Delete the 1-element, new values should be [-inf:0, 0:0, 10:2]
		c.erase(c.last(1));

		TESTEQUALS(c.last(1)->value, 0);
		TESTEQUALS(c.last(5)->value, 0);
		TESTEQUALS(c.last(47)->value, 2);

		// should do nothing, since we delete all at > 99,
		// but the last element is at 10. should still be [-inf:0, 0:0, 10:2]
		c.erase_after(c.last(99));
		TESTEQUALS(c.last(47)->value, 2);

		// now since 5 < 10, element with value 2 has to be gone
		// result should be [-inf:0, 0:0]
		c.erase_after(c.last(5));
		TESTEQUALS(c.last(47)->value, 0);

		c.insert_overwrite(0, 42);
		TESTEQUALS(c.last(100)->value, 42);
		TESTEQUALS(c.last(100)->time, 0);

		// the curve now contains [-inf:0, 0:42]
		// let's change/add some more elements
		c.insert_overwrite(0, 10);
		TESTEQUALS(c.last(100)->value, 10);

		c.insert_after(0, 11);
		c.insert_after(0, 12);
		// now: [-inf:0, 0:10, 0:11, 0:12]
		TESTEQUALS(c.last(0)->value, 12);
		TESTEQUALS(c.last(10)->value, 12);

		c.insert_before(0, 2);
		// all the values at t=0 should be 2, 10, 11, 12

		c.insert_after(1, 15);
		TESTEQUALS(c.last(1)->value, 15);
		TESTEQUALS(c.last(10)->value, 15);

		c.insert_overwrite(2, 20);
		TESTEQUALS(c.last(1)->value, 15);
		TESTEQUALS(c.last(2)->value, 20);
		TESTEQUALS(c.last(10)->value, 20);

		c.insert_before(3, 25);
		TESTEQUALS(c.last(1)->value, 15);
		TESTEQUALS(c.last(2)->value, 20);
		TESTEQUALS(c.last(3)->value, 25);
		TESTEQUALS(c.last(10)->value, 25);

		// now it should be [-inf: 0, 0: 2, 0: 10, 0: 11, 0: 12, 1: 15, 2: 20,
		// 3: 25]

		{
			auto it = c.begin();
			TESTEQUALS(it->time, std::numeric_limits<time_t>::min());
			TESTEQUALS(it->value, 0);

			TESTEQUALS((++it)->time, 0);
			TESTEQUALS(it->value, 2);

			TESTEQUALS((++it)->time, 0);
			TESTEQUALS(it->value, 10);

			TESTEQUALS((++it)->time, 0);
			TESTEQUALS(it->value, 11);

			TESTEQUALS((++it)->time, 0);
			TESTEQUALS(it->value, 12);

			TESTEQUALS((++it)->time, 1);
			TESTEQUALS(it->value, 15);

			TESTEQUALS((++it)->time, 2);
			TESTEQUALS(it->value, 20);

			TESTEQUALS((++it)->time, 3);
			TESTEQUALS(it->value, 25);
		}

		// TODO test c.insert_overwrite and c.insert_after
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
		c.set_insert(0, 0);
		c.set_insert(20, 20);

		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 1);
		TESTEQUALS(c.get(7), 7);

		c.set_last(20, 10);
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
		TESTEQUALS(c.get(15), 10);

		c.erase(10);
		TESTEQUALS(c.get(15), 0);

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
		c.set_insert(0, 1);
		c.set_insert(1, 1);
		c.set_last(1, -5);

		// [0:1, 1:-5]
		TESTEQUALS(c.get(1), -5);

		c.set_replace(1, 10);
		// [0:1, 1:10]
		TESTEQUALS(c.get(1), 10);

		c.set_insert(20, 20);
		c.set_insert(1, 1);
		c.set_insert(2, 2);

		// [0:1, 1:1, 2:2, 20:20]
		TESTEQUALS(c.get(0.5), 1);
		TESTEQUALS(c.get(1), 1);

		TESTEQUALS(c.get(5), 5);
		TESTEQUALS(c.get(20), 20);
		TESTEQUALS(c.get(25), 20);

		c.set_insert(0, 0);
		c.set_insert(1, 10);
		c.set_insert(20, 21);
		c.erase(2);
		// [0:0, 1:10, 20:21]
		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 10);
		// 10 + (20 - 10)/(21 - 1) * (5 - 1) = 12
		TESTEQUALS(c.get(5), 12);
		TESTEQUALS(c.get(0.5), 5);
	}

	// check jumps of Segmented
	{
		auto f = std::make_shared<event::Loop>();
		Segmented<int> c(f, 0);

		c.set_insert(0, 0);
		c.set_insert(2, 2);
		c.set_insert(10, 10);

		// [0:0, 2:2, 10:10]
		TESTEQUALS(c.get(5), 5);
		TESTEQUALS(c.get(1), 1);

		c.set_insert(1, 10);
		// [0:0, 1:10, 2:2, 10:10]
		TESTNOEXCEPT(c.check_integrity());
		TESTEQUALS(c.get(1), 10);
		TESTEQUALS(c.get(5), 5);

		c.set_insert_jump(1, 0, 20);
		// [0:0, 1:0, 1:20, 2:2, 10:10]
		TESTNOEXCEPT(c.check_integrity());
		TESTEQUALS(c.get(0.5), 0);
		TESTEQUALS(c.get(1), 20);

		c.erase(2);
		// [0:0, 1:0, 1:20, 10:10]
		TESTNOEXCEPT(c.check_integrity());
		c.str();
		TESTEQUALS(c.get(5), 15);

		c.set_last_jump(1, 4, 10);
		// [0:0, 1:4, 1:10]
		TESTNOEXCEPT(c.check_integrity());
		TESTEQUALS(c.get(0.5), 2);
		TESTEQUALS(c.get(1), 10);
		TESTEQUALS(c.get(5), 10);

		c.erase(1);
		// [0:0]
		TESTEQUALS(c.get(1), 0);
		TESTEQUALS(c.get(5), 0);
	}
}

} // openage::curve::tests
