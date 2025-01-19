// Copyright 2017-2025 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include "curve/container/array.h"
#include "curve/container/iterator.h"
#include "curve/container/map.h"
#include "curve/container/map_filter_iterator.h"
#include "curve/container/queue.h"
#include "curve/container/queue_filter_iterator.h"
#include "event/event_loop.h"
#include "testing/testing.h"


namespace openage::curve::tests {

struct map_test_element {
	int value;

	explicit map_test_element(int v) :
		value(v) {}

	bool operator!=(int rhs) {
		return this->value != rhs;
	}
};

std::ostream &operator<<(std::ostream &o, const map_test_element &e) {
	o << e.value;
	return o;
}

template <typename key_t, typename val_t>
void dump(const std::unordered_map<key_t, val_t> &map) {
	for (const auto &i : map) {
		std::cout << i.first << ": " << i.second << std::endl;
	}
}

void test_map() {
	static_assert(std::is_copy_constructible<MapFilterIterator<int, int, UnorderedMap<int, int>>>::value,
	              "UnorderedMapIterator not Copy Constructable able");
	static_assert(std::is_copy_assignable<MapFilterIterator<int, int, UnorderedMap<int, int>>>::value,
	              "UnorderedMapIterator not Copy Assignable");

	UnorderedMap<int, int> map;
	map.insert(0, 10, 0, 0);
	map.insert(5, 10, 5, 1);
	map.insert(100, 200, 200, 2);

	// Basic tests test lookup in the middle of the range.
	{
		auto t = map.at(2, 0); // At timestamp 2 element 0
		TESTEQUALS(t.has_value(), true);
		TESTEQUALS(t.value().value(), 0);
		t = map.at(20, 5);
		TESTEQUALS(t.has_value(), false);
	}
	{
		auto t = map.at(7, 5);
		TESTEQUALS(t.has_value(), true);
		TESTEQUALS(t.value().value(), 1);
		t = map.at(20, 5);
		TESTEQUALS(t.has_value(), false);
		t = map.at(2, 5);
		TESTEQUALS(t.has_value(), false);
	}
	{
		auto t = map.at(150, 200);
		TESTEQUALS(t.has_value(), true);
		TESTEQUALS(t.value().value(), 2);
		t = map.at(500, 200);
		TESTEQUALS(t.has_value(), false);
		t = map.at(5, 200);
		TESTEQUALS(t.has_value(), false);
	}
	// test 2.0: test at the boundaries
	{
		auto t = map.at(0, 0);
		TESTEQUALS(t.has_value(), true);
		TESTEQUALS(t.value().value(), 0);
		t = map.at(10, 0);
		TESTEQUALS(t.has_value(), false);
	}
	{
		auto t = map.at(5, 5);
		TESTEQUALS(t.has_value(), true);
		TESTEQUALS(t.value().value(), 1);
		t = map.at(10, 5);
		TESTEQUALS(t.has_value(), false);
	}
	{
		auto t = map.at(100, 200);
		TESTEQUALS(t.has_value(), true);
		TESTEQUALS(t.value().value(), 2);
		t = map.at(200, 200);
		TESTEQUALS(t.has_value(), false);
	}
	// Test 3.0 Iterations
	{
		// Iteration tests
		std::unordered_map<int, int> reference;
		reference[0] = 0;
		reference[5] = 1;
		reference[200] = 2;
		for (auto it = map.begin(0); it != map.end(); ++it) { // Get all
			auto ri = reference.find(it.key());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);

		reference[5] = 5;
		for (auto it = map.begin(1); it != map.end(90); ++it) {
			auto ri = reference.find(it.key());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);

		reference[5] = 5;
		for (auto it = map.between(1, 90); it != map.end(); ++it) {
			auto ri = reference.find(it.key());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
}

void test_list() {
}

void test_queue() {
	static_assert(std::is_copy_constructible<QueueFilterIterator<int, Queue<int>>>::value,
	              "QueueIterator not Copy Constructable able");
	static_assert(std::is_copy_assignable<QueueFilterIterator<int, Queue<int>>>::value,
	              "QueueIterator not Copy Assignable");

	auto loop = std::make_shared<event::EventLoop>();

	Queue<int> q{loop, 0};

	TESTEQUALS(q.empty(0), true);
	TESTEQUALS(q.empty(1), true);
	TESTEQUALS(q.empty(100001), true);

	q.insert(2, 2);
	q.insert(4, 3);
	q.insert(10, 4);
	q.insert(100001, 5);
	q.insert(100001, 6);

	TESTEQUALS(q.empty(0), true);
	TESTEQUALS(q.empty(1), true);
	TESTEQUALS(q.empty(2), false);
	TESTEQUALS(q.empty(100001), false);
	TESTEQUALS(q.empty(100002), false);

	q.insert(0, 1);

	TESTEQUALS(*q.begin(0), 1);
	TESTEQUALS(*q.begin(1), 2);
	TESTEQUALS(*q.begin(2), 2);
	TESTEQUALS(*q.begin(3), 3);
	TESTEQUALS(*q.begin(4), 3);
	TESTEQUALS(*q.begin(5), 4);
	TESTEQUALS(*q.begin(10), 4);
	TESTEQUALS(*q.begin(12), 5);
	TESTEQUALS(*q.begin(100000), 5);

	{
		std::unordered_set<int> reference = {1, 2, 3};
		for (auto it = q.between(0, 6); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
	{
		std::unordered_set<int> reference = {2, 3, 4};
		for (auto it = q.between(1, 40); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
	{
		std::unordered_set<int> reference = {};
		for (auto it = q.between(30, 40); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
	{
		std::unordered_set<int> reference = {1, 2, 3, 4};
		for (auto it = q.between(0, 40); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}

	TESTEQUALS(q.front(0), 1);
	TESTEQUALS(q.front(2), 1);
	TESTEQUALS(q.front(5), 1);
	TESTEQUALS(q.front(10), 1);
	TESTEQUALS(q.front(100001), 1);

	TESTEQUALS(q.pop_front(0), 1);
	TESTEQUALS(q.empty(0), true);

	TESTEQUALS(q.pop_front(12), 2);
	TESTEQUALS(q.empty(12), false);

	TESTEQUALS(q.pop_front(12), 3);
	TESTEQUALS(q.empty(12), false);

	TESTEQUALS(q.pop_front(12), 4);
	TESTEQUALS(q.empty(12), true);

	q.clear(0);
	TESTEQUALS(q.empty(0), true);
	TESTEQUALS(q.empty(100001), false);
}

void test_array() {
	auto loop = std::make_shared<event::EventLoop>();

	Array<int, 4> a(loop, 0);
	a.set_insert(1, 0, 0);
	a.set_insert(1, 1, 1);
	a.set_insert(1, 2, 2);
	a.set_insert(1, 3, 3);
	// a = [[0:0, 1:0],[0:0, 1:1],[0:0, 1:2],[0:0, 1:3]]

	// test size
	TESTEQUALS(a.size(), 4);

	// extracting array at time t == 1
	auto res = a.get(1);
	auto expected = std::array<int, 4>{0, 1, 2, 3};
	TESTEQUALS(res.at(0), expected.at(0));
	TESTEQUALS(res.at(1), expected.at(1));
	TESTEQUALS(res.at(2), expected.at(2));
	TESTEQUALS(res.at(3), expected.at(3));
	TESTEQUALS(res.size(), expected.size());

	// extracting array at time t == 0
	// array should have default values (== 0) for all keyframes
	res = a.get(0);
	TESTEQUALS(res.at(0), 0);
	TESTEQUALS(res.at(1), 0);
	TESTEQUALS(res.at(2), 0);
	TESTEQUALS(res.at(3), 0);

	Array<int, 4> other(loop, 0);
	other.set_last(0, 0, 999);
	other.set_last(0, 1, 999);
	other.set_last(0, 2, 999);
	other.set_last(0, 3, 999);

	// inserting keyframes at time t == 1
	other.set_insert(1, 0, 4);
	other.set_insert(1, 1, 5);
	other.set_insert(1, 2, 6);
	other.set_insert(1, 3, 7);
	// other = [[0:999, 1:4],[0:999, 1:5],[0:999, 1:6],[0:999, 1:7]]

	TESTEQUALS(other.at(0, 0), 999);
	TESTEQUALS(other.at(0, 1), 999);
	TESTEQUALS(other.at(0, 2), 999);
	TESTEQUALS(other.at(0, 3), 999);
	TESTEQUALS(other.at(1, 0), 4);
	TESTEQUALS(other.at(1, 1), 5);
	TESTEQUALS(other.at(1, 2), 6);
	TESTEQUALS(other.at(1, 3), 7);

	// sync keyframes from other to a
	a.sync(other, 1);
	// a = [[0:0, 1:4],[0:0, 1:5],[0:0, 1:6],[0:0, 1:7]]

	res = a.get(0);
	TESTEQUALS(res.at(0), 0);
	TESTEQUALS(res.at(1), 0);
	TESTEQUALS(res.at(2), 0);
	TESTEQUALS(res.at(3), 0);
	res = a.get(1);
	TESTEQUALS(res.at(0), 4);
	TESTEQUALS(res.at(1), 5);
	TESTEQUALS(res.at(2), 6);
	TESTEQUALS(res.at(3), 7);

	// replace keyframes at time t == 2
	a.set_insert(2, 0, 15);
	a.set_insert(2, 0, 20);
	a.set_replace(2, 0, 25);
	TESTEQUALS(a.at(2, 0), 25);
	// a = [[0:0, 1:4, 2:25],[0:0, 1:5],[0:0, 1:6],[0:0, 1:7]]

	// set last keyframe at time t == 3
	a.set_insert(3, 0, 30); // a = [[0:0, 1:4, 2:25, 3:30], ...
	a.set_insert(4, 0, 40); // a = [[0:0, 1:4, 2:25, 3:30, 4:40], ...
	a.set_last(3, 0, 35);   // a = [[0:0, 1:4, 2:25, 3:35],...
	TESTEQUALS(a.at(4, 0), 35);
	// a = [[0:0, 1:4, 2:25, 3:35],[0:0, 1:5],[0:0, 1:6],[0:0, 1:7]]

	// test frame and next_frame
	auto frame = a.frame(1, 2);
	TESTEQUALS(frame.first, 1);  // time
	TESTEQUALS(frame.second, 6); // value

	a.set_insert(5, 3, 40);
	// a = [[0:0, 1:4, 2:25, 3:35],[0:0, 1:5],[0:0, 1:6],[0:0, 1:7, 5:40]]
	auto next_frame = a.next_frame(1, 3);
	TESTEQUALS(next_frame.first, 5);   // time
	TESTEQUALS(next_frame.second, 40); // value

	// Test begin and end
	auto it = a.begin(1);
	TESTEQUALS(*it, 4);
	++it;
	TESTEQUALS(*it, 5);
	++it;
	TESTEQUALS(*it, 6);
	++it;
	TESTEQUALS(*it, 7);
}


void container() {
	test_map();
	test_list();
	test_queue();
	test_array();
}


} // namespace openage::curve::tests
