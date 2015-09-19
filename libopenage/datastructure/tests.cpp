// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <utility>

#include "../testing/testing.h"

#include "constexpr_map.h"
#include "doubly_linked_list.h"
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
	(heap.size() == 5) or TESTFAIL;
	(heap.top() == 0) or TESTFAIL;

	(0 == heap.pop()) or TESTFAIL;
	(1 == heap.pop()) or TESTFAIL;
	(2 == heap.pop()) or TESTFAIL;
	(3 == heap.pop()) or TESTFAIL;

	(heap.size() == 1) or TESTFAIL;

	heap.push(0);
	heap.push(10);

	// state: 0 4 10

	(0 == heap.pop()) or TESTFAIL;
	(4 == heap.pop()) or TESTFAIL;
	(10 == heap.pop()) or TESTFAIL;
	(heap.size() == 0) or TESTFAIL;

	heap.push(5);
	heap.push(5);
	heap.push(0);
	heap.push(5);
	heap.push(5);
	(0 == heap.pop()) or TESTFAIL;
	(5 == heap.pop()) or TESTFAIL;
	(5 == heap.pop()) or TESTFAIL;
	(5 == heap.pop()) or TESTFAIL;
	(5 == heap.pop()) or TESTFAIL;

	(heap.size() == 0) or TESTFAIL;
}


void pairing_heap_1() {
	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{1});
	auto node = heap.push(heap_elem{2});
	heap.push(heap_elem{3});

	// 1 2 3
	node->data.data = 0;
	heap.update(node);

	// 0 1 3
	(0 == heap.pop().data) or TESTFAIL;
	(1 == heap.pop().data) or TESTFAIL;
	(3 == heap.pop().data) or TESTFAIL;
}


void pairing_heap_2() {
	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{1});
	auto node = heap.push(heap_elem{2});
	heap.push(heap_elem{3});

	// state: 1 2 3
	(heap.pop_node(node).data == 2) or TESTFAIL;

	// state: 1 3
	(1 == heap.pop().data) or TESTFAIL;
	(3 == heap.pop().data) or TESTFAIL;
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

	(heap.size() == 0) or TESTFAIL;
	(heap.empty() == true) or TESTFAIL;
}


// exported test
void pairing_heap() {
	pairing_heap_0();
	pairing_heap_1();
	pairing_heap_2();
	pairing_heap_3();
}


// exported test
void doubly_linked_list() {
	datastructure::DoublyLinkedList<int> list;

	list.empty() or TESTFAIL;

	list.push_front(0);
	list.push_front(1);
	list.push_front(2);

	list.push_back(3);
	list.push_front(4);
	list.push_back(5);

	// 421035

	(list.size() == 6) or TESTFAIL;
	(list.pop_back() == 5) or TESTFAIL;
	(list.pop_front() == 4) or TESTFAIL;

	// 2103
	(list.size() == 4) or TESTFAIL;

	list.push_back(6);
	list.push_front(7);
	list.push_back(8);

	// 7210368
	(8 == list.pop_back()) or TESTFAIL;
	(6 == list.pop_back()) or TESTFAIL;
	(3 == list.pop_back()) or TESTFAIL;
	(7 == list.pop_front()) or TESTFAIL;
	(2 == list.pop_front()) or TESTFAIL;
	(0 == list.pop_back()) or TESTFAIL;
	(1 == list.pop_back()) or TESTFAIL;

	(list.size() == 0) or TESTFAIL;
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

	static_assert(create_const_map<int, int>(std::make_pair(42, 9001),
	                                         std::make_pair(13, 37)).get(42) == 9001,
	              "fetched wrong value");
	static_assert(create_const_map<int, int>(std::make_pair(42, 9001),
	                                         std::make_pair(13, 37)).get(13) == 37,
	              "fetched wrong value");

	constexpr auto cmap = create_const_map<int, int>(
		std::make_pair(0, 0),
		std::make_pair(13, 37),
		std::make_pair(42, 9001)
	);

	cmap.contains(0) or TESTFAIL;
	not cmap.contains(18) or TESTFAIL;

	cmap.size() == 3 or TESTFAIL;
	cmap.get(13) == 37 or TESTFAIL;
	cmap.get(42) == 9001 or TESTFAIL;
}

}}} // openage::datastructure::tests
