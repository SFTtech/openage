// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#include <iterator>
#include <list>
#include <memory>
#include <string>

#include "curve/continuous.h"
#include "curve/discrete.h"
#include "curve/discrete_mod.h"
#include "curve/keyframe.h"
#include "curve/keyframe_container.h"
#include "curve/segmented.h"
#include "event/event_loop.h"
#include "testing/testing.h"
#include "time/time.h"
#include "util/fixed_point.h"

namespace openage::curve::tests {

void curve_types() {
	// Check the base container type
	{
		auto loop = std::make_shared<event::EventLoop>();
		KeyframeContainer<int> c;

		auto p0 = c.insert_before(0, 0);
		auto p1 = c.insert_before(1, 1);
		auto p2 = c.insert_before(10, 2);
		auto ib = 0;
		auto ie = c.size();

		// now contains: [-inf: 0, 0:0, 1:1, 10:2]

		TESTEQUALS(c.size(), 4);

		{
			auto it = c.begin();
			TESTEQUALS(it->val(), 0);
			TESTEQUALS(it->time(), time::TIME_MIN);
			TESTEQUALS((++it)->time(), 0);
			TESTEQUALS(it->val(), 0);
			TESTEQUALS((++it)->time(), 1);
			TESTEQUALS(it->val(), 1);
			TESTEQUALS((++it)->time(), 10);
			TESTEQUALS(it->val(), 2);
		}

		// last function tests without hints
		TESTEQUALS(c.get(c.last(0)).val(), 0);
		TESTEQUALS(c.get(c.last(1)).val(), 1);
		TESTEQUALS(c.get(c.last(5)).val(), 1);
		TESTEQUALS(c.get(c.last(10)).val(), 2);
		TESTEQUALS(c.get(c.last(47)).val(), 2);

		// last() with hints.
		TESTEQUALS(c.get(c.last(0, ib)).val(), 0);
		TESTEQUALS(c.get(c.last(1, ib)).val(), 1);
		TESTEQUALS(c.get(c.last(5, ib)).val(), 1);
		TESTEQUALS(c.get(c.last(10, ib)).val(), 2);
		TESTEQUALS(c.get(c.last(47, ib)).val(), 2);

		TESTEQUALS(c.get(c.last(0, p0)).val(), 0);
		TESTEQUALS(c.get(c.last(1, p0)).val(), 1);
		TESTEQUALS(c.get(c.last(5, p0)).val(), 1);
		TESTEQUALS(c.get(c.last(10, p0)).val(), 2);
		TESTEQUALS(c.get(c.last(47, p0)).val(), 2);

		TESTEQUALS(c.get(c.last(0, p1)).val(), 0);
		TESTEQUALS(c.get(c.last(1, p1)).val(), 1);
		TESTEQUALS(c.get(c.last(5, p1)).val(), 1);
		TESTEQUALS(c.get(c.last(10, p1)).val(), 2);
		TESTEQUALS(c.get(c.last(47, p1)).val(), 2);

		TESTEQUALS(c.get(c.last(0, p2)).val(), 0);
		TESTEQUALS(c.get(c.last(1, p2)).val(), 1);
		TESTEQUALS(c.get(c.last(5, p2)).val(), 1);
		TESTEQUALS(c.get(c.last(10, p2)).val(), 2);
		TESTEQUALS(c.get(c.last(47, p2)).val(), 2);

		TESTEQUALS(c.get(c.last(0, ie)).val(), 0);
		TESTEQUALS(c.get(c.last(1, ie)).val(), 1);
		TESTEQUALS(c.get(c.last(5, ie)).val(), 1);
		TESTEQUALS(c.get(c.last(10, ie)).val(), 2);
		TESTEQUALS(c.get(c.last(47, ie)).val(), 2);

		// Now test the basic erase() function
		// Delete the 1-element, new values should be [-inf:0, 0:0, 10:2]
		c.erase(c.last(1));

		TESTEQUALS(c.get(c.last(1)).val(), 0);
		TESTEQUALS(c.get(c.last(5)).val(), 0);
		TESTEQUALS(c.get(c.last(47)).val(), 2);

		// should do nothing, since we delete all at > 99,
		// but the last element is at 10. should still be [-inf:0, 0:0, 10:2]
		c.erase_after(c.last(99));
		TESTEQUALS(c.get(c.last(47)).val(), 2);

		// now since 5 < 10, element with value 2 has to be gone
		// result should be [-inf:0, 0:0]
		c.erase_after(c.last(5));
		TESTEQUALS(c.get(c.last(47)).val(), 0);

		c.insert_overwrite(0, 42);
		TESTEQUALS(c.get(c.last(100)).val(), 42);
		TESTEQUALS(c.get(c.last(100)).time(), 0);

		// the curve now contains [-inf:0, 0:42]
		// let's change/add some more elements
		c.insert_overwrite(0, 10);
		TESTEQUALS(c.get(c.last(100)).val(), 10);

		c.insert_after(0, 11);
		c.insert_after(0, 12);
		// now: [-inf:0, 0:10, 0:11, 0:12]
		TESTEQUALS(c.get(c.last(0)).val(), 12);
		TESTEQUALS(c.get(c.last(10)).val(), 12);

		c.insert_before(0, 2);
		// all the values at t=0 should be 2, 10, 11, 12

		c.insert_after(1, 15);
		TESTEQUALS(c.get(c.last(1)).val(), 15);
		TESTEQUALS(c.get(c.last(10)).val(), 15);

		c.insert_overwrite(2, 20);
		TESTEQUALS(c.get(c.last(1)).val(), 15);
		TESTEQUALS(c.get(c.last(2)).val(), 20);
		TESTEQUALS(c.get(c.last(10)).val(), 20);

		c.insert_before(3, 25);
		TESTEQUALS(c.get(c.last(1)).val(), 15);
		TESTEQUALS(c.get(c.last(2)).val(), 20);
		TESTEQUALS(c.get(c.last(3)).val(), 25);
		TESTEQUALS(c.get(c.last(10)).val(), 25);

		// now it should be [-inf: 0, 0: 2, 0: 10, 0: 11, 0: 12, 1: 15, 2: 20,
		// 3: 25]

		{
			auto it = c.begin();
			TESTEQUALS(it->time(), time::TIME_MIN);
			TESTEQUALS(it->val(), 0);

			TESTEQUALS((++it)->time(), 0);
			TESTEQUALS(it->val(), 2);

			TESTEQUALS((++it)->time(), 0);
			TESTEQUALS(it->val(), 10);

			TESTEQUALS((++it)->time(), 0);
			TESTEQUALS(it->val(), 11);

			TESTEQUALS((++it)->time(), 0);
			TESTEQUALS(it->val(), 12);

			TESTEQUALS((++it)->time(), 1);
			TESTEQUALS(it->val(), 15);

			TESTEQUALS((++it)->time(), 2);
			TESTEQUALS(it->val(), 20);

			TESTEQUALS((++it)->time(), 3);
			TESTEQUALS(it->val(), 25);
		}

		// TODO: test c.insert_overwrite and c.insert_after

		KeyframeContainer<int> c2;
		c2.sync(c, 1);
		// now c2 should be [-inf: 0, 1: 15, 2: 20, 3: 25]
		TESTEQUALS(c2.get(c2.last(0)).val(), 0);
		TESTEQUALS(c2.get(c2.last(1)).val(), 15);
		TESTEQUALS(c2.get(c2.last(2)).val(), 20);
		TESTEQUALS(c2.get(c2.last(3)).val(), 25);
		TESTEQUALS(c2.get(c2.last(10)).val(), 25);
		TESTEQUALS(c2.size(), 4);

		c.clear();
		// now it should be [-inf: 0]
		TESTEQUALS(c.get(c.last(0)).val(), 0);
		TESTEQUALS(c.get(c.last(1)).val(), 0);
		TESTEQUALS(c.size(), 1);
	}

	// Check the Simple Continuous type
	{
		auto f = std::make_shared<event::EventLoop>();
		Continuous<float> c(f, 0);

		c.set_insert(0, 0);
		c.set_insert(10, 1);

		TESTEQUALS(c.get(0), 0);

		TESTEQUALS_FLOAT(c.get(1), 0.1, 1e-7);

		Continuous<float> c2(f, 0);
		c2.sync(c, 0);

		c2.set_insert(0, 5);
		c2.set_insert(10, 0);

		TESTEQUALS(c2.get(0), 5);
		TESTEQUALS(c2.get(10), 0);

		TESTEQUALS_FLOAT(c2.get(1), 4.5, 1e-7);

		c2.sync(c, 5);
		TESTEQUALS(c2.get(10), 1);

		// for t >= 5 c and c2 should have the same values after sync
		TESTEQUALS_FLOAT(c.get(5), c2.get(5), 1e-7);
		TESTEQUALS_FLOAT(c.get(7), c2.get(7), 1e-7);
		TESTEQUALS_FLOAT(c.get(10), c2.get(10), 1e-7);
	}

	{
		auto f = std::make_shared<event::EventLoop>();
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

	// Check the discrete type
	{
		auto f = std::make_shared<event::EventLoop>();
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

	// Check the discrete mod type
	{
		auto f = std::make_shared<event::EventLoop>();
		DiscreteMod<int> c(f, 0);
		c.set_insert(0, 0);
		c.set_insert(5, 20);
		c.set_insert(10, 10);

		TESTEQUALS(c.get(0), 0);
		TESTEQUALS(c.get(1), 0);
		TESTEQUALS(c.get(5), 20);
		TESTEQUALS(c.get(10), 10);
		TESTEQUALS(c.get(15), 10);

		TESTEQUALS(c.get_mod(0, 0), 0);
		TESTEQUALS(c.get_mod(1, 0), 0);
		TESTEQUALS(c.get_mod(5, 0), 20);

		// wraparound
		TESTEQUALS(c.get_mod(10, 0), 0);
		TESTEQUALS(c.get_mod(11, 0), 0);
		TESTEQUALS(c.get_mod(16, 0), 20);

		// start offsets
		TESTEQUALS(c.get_mod(101, 100), 0);
		TESTEQUALS(c.get_mod(1337, 1000), 20);

		c.erase(10);
		TESTEQUALS(c.get(15), 20);
		TESTEQUALS(c.get_mod(5, 0), 0);
		TESTEQUALS(c.get_mod(15, 0), 0);
	}

	// check set_last
	{
		auto f = std::make_shared<event::EventLoop>();
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
		auto f = std::make_shared<event::EventLoop>();
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
		auto f = std::make_shared<event::EventLoop>();
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

} // namespace openage::curve::tests
