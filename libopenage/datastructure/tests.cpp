// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <utility>

#include "../testing/testing.h"

#include "constexpr_map.h"
#include "pairing_heap.h"


namespace openage {
namespace datastructure {
namespace tests {


void pairing_heap_0() {
	PairingHeap<int> heap{};

	(heap.size() == 0) or TESTFAIL;

	heap.push(0);
	heap.push(1);
	heap.push(2);
	heap.push(3);
	heap.push(4);

	// state: 01234
	TESTEQUALS(heap.size(), 5);
	TESTEQUALS(heap.top(), 0);

	TESTEQUALS(heap.pop(), 0);
	TESTEQUALS(heap.pop(), 1);
	TESTEQUALS(heap.pop(), 2);
	TESTEQUALS(heap.pop(), 3);

	TESTEQUALS(heap.size(), 1);

	heap.push(0);
	heap.push(10);

	// state: 0 4 10

	TESTEQUALS(heap.pop(), 0);
	TESTEQUALS(heap.pop(), 4);
	TESTEQUALS(heap.pop(), 10);
	TESTEQUALS(heap.size(), 0);

	heap.push(5);
	heap.push(5);
	heap.push(0);
	heap.push(5);
	heap.push(5);
	TESTEQUALS(heap.pop(), 0);
	TESTEQUALS(heap.pop(), 5);
	TESTEQUALS(heap.pop(), 5);
	TESTEQUALS(heap.pop(), 5);
	TESTEQUALS(heap.pop(), 5);

	TESTEQUALS(heap.size(), 0);
}


void pairing_heap_1() {
	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{1});
	auto node_u1 = heap.push(heap_elem{2});
	heap.push(heap_elem{3});

	// 1 2 3
	node_u1->data.data = 0;
	heap.decrease(node_u1);

	// 0 1 3
	TESTEQUALS(heap.pop().data, 0);
	TESTEQUALS(heap.pop().data, 1);

	// 3
	heap.push(heap_elem{4});
	heap.push(heap_elem{0});
	heap.push(heap_elem{2});
	heap.push(heap_elem{5});
	heap.push(heap_elem{1});
	heap.push(heap_elem{6});
	auto node_u2 = heap.push(heap_elem{7});
	heap.push(heap_elem{8});
	heap.push(heap_elem{9});
	heap.push(heap_elem{10});

	TESTEQUALS(heap.pop().data, 0);

	TESTEQUALS(heap.pop().data, 1);

	// now update the 7-node to 8
	node_u2->data.data = 8;
	heap.update(node_u2);

	// 2 3 4 5 6 8 8 9 10
	TESTEQUALS(heap.pop().data, 2);
	TESTEQUALS(heap.pop().data, 3);
	TESTEQUALS(heap.pop().data, 4);
	TESTEQUALS(heap.pop().data, 5);
	TESTEQUALS(heap.pop().data, 6);
	TESTEQUALS(heap.pop().data, 8);
	TESTEQUALS(heap.pop().data, 8);
	TESTEQUALS(heap.pop().data, 9);
	TESTEQUALS(heap.pop().data, 10);
}


void pairing_heap_2() {
	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{1});
	auto node = heap.push(heap_elem{2});
	heap.push(heap_elem{3});

	// state: 1 2 3, now remove 2
	heap.unlink_node(node);

	// state: 1 3
	TESTEQUALS(heap.pop().data, 1);
	TESTEQUALS(heap.pop().data, 3);
}


void pairing_heap_3() {
	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{0});
	heap.push(heap_elem{1});
	heap.push(heap_elem{2});
	heap.push(heap_elem{3});
	heap.push(heap_elem{4});
	heap.push(heap_elem{5});
	heap.pop(); // trigger pairing

	heap.clear();

	TESTEQUALS(heap.size(), 0);
	TESTEQUALS(heap.empty(), true);
}


// exported test
void pairing_heap() {
	pairing_heap_0();
	pairing_heap_1();
	pairing_heap_2();
	pairing_heap_3();
}


// exported test
void constexpr_map() {
	static_assert(create_const_map<int, int>().size() == 0, "wrong size");
	static_assert(create_const_map<int, int>(std::make_pair(0, 42)).size() == 1,
	              "wrong size");
	static_assert(create_const_map<int, int>(std::make_pair(0, 42),
	                                         std::make_pair(13, 37)).size() == 2,
	              "wrong size");

	static_assert(not create_const_map<int, int>().contains(9001),
	              "empty map doesn't contain anything");
	static_assert(create_const_map<int, int>(std::make_pair(42, 0),
	                                         std::make_pair(13, 37)).contains(42),
	              "contained element missing");
	static_assert(create_const_map<int, int>(std::make_pair(42, 0),
	                                         std::make_pair(13, 37)).contains(13),
	              "contained element missing");
	static_assert(not create_const_map<int, int>(std::make_pair(42, 0),
	                                             std::make_pair(13, 37)).contains(9001),
	              "uncontained element seems to be contained.");
#ifndef _MSC_VER // HACK: MSVC doesn't have full C++14 constexpr support
	static_assert(create_const_map<int, int>(std::make_pair(42, 9001),
	                                         std::make_pair(13, 37)).get(42) == 9001,
	              "fetched wrong value");
	static_assert(create_const_map<int, int>(std::make_pair(42, 9001),
	                                         std::make_pair(13, 37)).get(13) == 37,
	              "fetched wrong value");
#endif
	constexpr auto cmap = create_const_map<int, int>(
		std::make_pair(0, 0),
		std::make_pair(13, 37),
		std::make_pair(42, 9001)
	);

	cmap.contains(0) or TESTFAIL;
	not cmap.contains(18) or TESTFAIL;

	TESTEQUALS(cmap.size(), 3);
	TESTEQUALS(cmap.get(13), 37);
	TESTEQUALS(cmap.get(42), 9001);
}

}}} // openage::datastructure::tests
