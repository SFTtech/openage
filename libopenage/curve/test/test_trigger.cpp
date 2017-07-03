// Copyright 2017-2017 the openage authors. See copying.md for legal info.

// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../../testing/testing.h"
#include "../trigger.h"

#include <vector>
#include <string>

namespace openage {
namespace curve {
namespace tests {

void trigger() {
	{
		EventQueue q;
		TriggerFactory factory{&q};
		std::vector<std::string> lst;
		Trigger a (&factory);

		a.on_change(10, "change", [&lst](EventQueue *, const curve_time_t &) {
				lst.push_back("change");
			});

		a.on_pre_horizon(100, "horizon", [&lst](EventQueue *, const curve_time_t &) {
				lst.push_back("horizon");
			});

		a.on_pass_keyframe("keyframe", [&lst](EventQueue *, const curve_time_t &) {
				lst.push_back("keyframe");
			});

		a.on_change_future("future", [&lst](EventQueue *, const curve_time_t &) {
				lst.push_back("future");
			});

		a.data_changed(1, 50);

		q.execute_until(20);
		TESTEQUALS(lst.size(), 2);
		TESTEQUALS(lst[0], "future");
		TESTEQUALS(lst[1], "change");

		a.data_changed(2, 50);
		lst.clear();
		q.execute_until(20);
		TESTEQUALS(lst.size(), 1);
		TESTEQUALS(lst[0], "future");

		lst.clear();
		a.passed_keyframe(10);
		q.execute_until(20);
		TESTEQUALS(lst.size(), 1);
		TESTEQUALS(lst[0], "keyframe");

		lst.clear();
		q.execute_until(120);
		TESTEQUALS(lst.size(), 1);
		TESTEQUALS(lst[0], "horizon");
	}
}

}}} // openage::curve::tests
