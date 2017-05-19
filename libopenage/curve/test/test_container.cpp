// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../../testing/testing.h"
#include "../curve.h"
#include "../continuous.h"
#include "../discrete.h"
#include "../queue.h"
#include "../map.h"

#include <map>
#include <set>

namespace openage {
namespace curve {
namespace tests {

struct map_test_element {
	volatile int value;

	map_test_element(int v) :
		value(v) {}

	bool operator != (int rhs) {
		return this->value != rhs;
	}

};

std::ostream &operator << (std::ostream &o, const map_test_element &e) {
	o << e.value;
	return o;
}

template <typename key_t, typename val_t>
void dump(const std::map<key_t, val_t> &map) {
	for (auto i : map) {
		std::cout << i.first << ": " << i.second << std::endl;
	}
}

void test_map() {
	UnorderedMap<int, int> map;
	map.insert(0, 10, 0, 0);
	map.insert(5, 10, 5, 1);
	map.insert(100, 200, 200, 2);

	// Basic tests test lookup in the middle of the range.
	{
		auto t = map.at(2, 0); //At timestamp 2 element 0
		TESTEQUALS(t.first, true);
		TESTEQUALS(t.second.value(), 0);
		t = map.at(20, 5);
		TESTEQUALS(t.first, false);
	}
	{
		auto t = map.at(7, 5);
		TESTEQUALS(t.first, true);
		TESTEQUALS(t.second.value(), 1);
		t = map.at(20, 5);
		TESTEQUALS(t.first, false);
		t = map.at(2, 5);
		TESTEQUALS(t.first, false);
	}
	{
		auto t = map.at(150, 200);
		TESTEQUALS(t.first, true);
		TESTEQUALS(t.second.value(), 2);
		t = map.at(500, 200);
		TESTEQUALS(t.first, false);
		t = map.at(5, 200);
		TESTEQUALS(t.first, false);
	}
	// test 2.0: test at the boundaries
	{
		auto t = map.at(0, 0);
		TESTEQUALS(t.first, true);
		TESTEQUALS(t.second.value(), 0);
		t = map.at(10, 0);
		TESTEQUALS(t.first, false);
	}
	{
		auto t = map.at(5, 5);
		TESTEQUALS(t.first, true);
		TESTEQUALS(t.second.value(), 1);
		t = map.at(10, 5);
		TESTEQUALS(t.first, false);
	}
	{
		auto t = map.at(100, 200);
		TESTEQUALS(t.first, true);
		TESTEQUALS(t.second.value(), 2);
		t = map.at(200, 200);
		TESTEQUALS(t.first, false);
	}
	// Test 3.0 Iterations
	{
		// Iteration tests
		std::map<volatile int, int> reference;
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
		for (auto it = map.between(1,90); it != map.end(); ++it) {
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
	Queue<int> q;
	q.insert(0, 1);
	q.insert(2, 2);
	q.insert(4, 3);
	q.insert(10, 4);
	q.insert(100001, 5);
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
		std::set<int> reference = {1,2,3};
		for (auto it = q.between(0,6); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
	{
		std::set<int> reference = {2,3,4};
		for (auto it = q.between(1,40); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
	{
		std::set<int> reference = {};
		for (auto it = q.between(30,40); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}
	{
		std::set<int> reference = {1,2,3,4};
		for (auto it = q.between(0,40); it != q.end(); ++it) {
			auto ri = reference.find(it.value());
			if (ri != reference.end()) {
				reference.erase(ri);
			}
		}
		TESTEQUALS(reference.empty(), true);
	}

}


void container() {
	test_map();
	test_list();
	test_queue();
}


}}} // openage::curve::tests
