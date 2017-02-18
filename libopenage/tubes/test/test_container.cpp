// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../../testing/testing.h"
#include "../base.h"
#include "../container_iterator.h"
#include "../tube.h"
#include "../simple_continuous.h"
#include "../simple_discrete.h"

namespace openage {
namespace tube {
namespace tests {

class test_tube_element {
public:
	test_tube_element(tube_time_t t, int32_t data) :
		time(t),
		data(data)
	{}

	tube_time_t time;
	int32_t data;
};

tube_time_t timer (const test_tube_element &t) {
	return t.time;
}

void container() {
	// Check the base container type
	{
		tubebase<int> c;
		auto p0 = c.create(0, 0);
		auto p1 = c.create(1, 1);
		auto p2 = c.create(10, 2);

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
		SimpleContinuous<float> c;
		c.set_insert(0, 0);
		c.set_insert(10, 1);

		c.set_now(0);
		TESTEQUALS(c.get(), 0);

		c.set_now(1);
		TESTEQUALS_FLOAT(c.get(), 0.1, 1e-7);
	}

	{
		SimpleContinuous<int> c;
		c.set_insert(0, 0);
		c.set_insert(10, 10);

		c.set_now(0);
		TESTEQUALS(c.get(), 0);

		c.set_now(1);
		TESTEQUALS(c.get(), 1);
	}
	//Check the discrete type
	{
		SimpleDiscrete<int> c;
		c.set_insert(0, 0);
		c.set_insert(10, 10);

		c.set_now(0);
		TESTEQUALS(c.get(), 0);

		c.set_now(1);
		TESTEQUALS(c.get(), 0);

		c.set_now(11);
		TESTEQUALS(c.get(), 10);
	}

	//check set_drop
	{
		SimpleDiscrete<int> c;
		c.set_insert(0, 0);
		c.set_insert(1, 1);
		c.set_insert(3, 3);

		c.set_now(3);
		TESTEQUALS(c.get(), 3);

		c.set_drop(2, 10);
		c.set_now(3);
		TESTEQUALS(c.get(), 10);
	}

	// Iterate tests
/*	{
		Container<test_tube_element> container(timer);

		container.insert(std::make_shared<test_tube_element>(test_tube_element(10, 0)));
		container.insert(std::make_shared<test_tube_element>(test_tube_element(20, 1)));

		// Iterate over everything with boundaries
		{
			auto it = container.iterate(0, 30);
			TESTEQUALS(it.valid(), true);

			TESTEQUALS(it->data, 0);
			++it;
			TESTEQUALS(it.valid(), true);

			TESTEQUALS(it->data, 1);
			++it;
			TESTEQUALS(it.valid(), false);

		}

		// Iterate only over the first, with boundaries
		{
			auto it = container.iterate(0, 15);
			TESTEQUALS(it.valid(), true);
			TESTEQUALS(it->data, 0);
			++it;
			TESTEQUALS(it.valid(), false);
		}

		// Iterate only over the first, without boundaries
		{
			auto it = container.iterate(10, 11);
			TESTEQUALS(it.valid(), true);
			TESTEQUALS(it->data, 0);
			++it;
			TESTEQUALS(it.valid(), false);
		}

		// Iterate out of boundaries
		{
			auto it = container.iterate(100, 200);
			TESTEQUALS(it.valid(), false);
		}
	}

	// Some fucked up insert and read stuff, create all the bugs.
	{
		// TODO have more ideas to fuck up our data structures
	}*/
}


}}} // openage::tube::tests
