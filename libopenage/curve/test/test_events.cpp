// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../../testing/testing.h"
#include "../curve.h"
#include "../events.h"

namespace openage {
namespace curve {
namespace tests {

static void print_vector(const std::vector<std::pair<curve_time_t, int>> &t) {
	for (const auto &a : t) {
		std::cout << a.first << ":" << a.second << std::endl;
	}
}

void events () {
	EventQueue q;

	std::vector<std::pair<curve_time_t, int>> calls;
	print_vector(calls);

	q.addcallback(1, [&calls](const curve_time_t &t) {
			calls.push_back(std::make_pair(t, 1));
		});

	q.addcallback(2, [&calls](const curve_time_t &t) {
			calls.push_back(std::make_pair(t, 2));
		});

	q.trigger(0, 2);
	TESTEQUALS(calls.size(), 1);
	TESTEQUALS(calls[0].first, 1);
	TESTEQUALS(calls[0].second, 1);

	calls.clear();
	q.trigger(0, 3);
	TESTEQUALS(calls.size(), 2);
	TESTEQUALS(calls[0].first, 1);
	TESTEQUALS(calls[0].second, 1);
	TESTEQUALS(calls[1].first, 2);
	TESTEQUALS(calls[1].second, 2);

	calls.clear();
	q.trigger(2, 3);
	TESTEQUALS(calls.size(), 1);
	TESTEQUALS(calls[0].first, 2);
	TESTEQUALS(calls[0].second, 2);

	int cond1 = 1;
	q.addcallback(4, [&calls, cond1](const curve_time_t &) {
			return cond1 == 1;
		},
		[&calls](const curve_time_t &t) {
			calls.push_back(std::make_pair(t, 4));
		});

	cond1 = 1;
	calls.clear();
	q.trigger(4, 5);
	TESTEQUALS(calls.size(), 1);
	TESTEQUALS(calls[0].first, 4);
	TESTEQUALS(calls[0].second, 4);

	cond1 = 0;
	calls.clear();
	TESTEQUALS(calls.size(), 0);

	int cond2 = 1;
	q.addcallback(5, [&cond2](const curve_time_t &) {
			return cond2 == 1;
		},
		[&calls](const curve_time_t &t) {
			calls.push_back(std::make_pair(t, 5));
		});

	q.addcallback(6, [&cond2](const curve_time_t &) {
			return cond2 == 1 || cond2 == 2;
		},
		[&calls, &cond2] (const curve_time_t &t) {
			calls.push_back(std::make_pair(t, 6));
			cond2 ++;
		});

	cond2 = 1;
	calls.clear();
	q.trigger(5, 7);
	TESTEQUALS(calls.size(), 2);
	TESTEQUALS(calls[0].first, 5);
	TESTEQUALS(calls[0].second, 5);
	TESTEQUALS(calls[1].first, 6);
	TESTEQUALS(calls[1].second, 6);
	TESTEQUALS(cond2, 2);

	calls.clear();
	q.trigger(5, 7);
	TESTEQUALS(calls.size(), 1);
	TESTEQUALS(calls[0].first, 6);
	TESTEQUALS(calls[0].second, 6);
	TESTEQUALS(cond2, 3);

	calls.clear();
	q.trigger(5, 7);
	TESTEQUALS(calls.size(), 0);
	TESTEQUALS(cond2, 3);
}

}}} // namespace::curve::tests
