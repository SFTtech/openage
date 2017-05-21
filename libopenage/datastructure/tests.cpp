// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <utility>
#include <thread>

#include "../testing/testing.h"

#include "constexpr_map.h"
#include "pairing_heap.h"
#include "concurrent_double_buffer.h"


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

void concurrent_double_buffer_ctr() {
	ConcurrentDoubleBuffer<int>();
	ConcurrentDoubleBuffer<int>(2, 3);

	static bool deleted = false;
	{
		struct Foo {
			~Foo() {
				deleted = true;
			}
		};
		ConcurrentDoubleBuffer<std::unique_ptr<Foo>>(std::make_unique<Foo>(), std::make_unique<Foo>());
	}
	deleted || TESTFAIL;
}

void concurrent_double_buffer_st() {
	ConcurrentDoubleBuffer<int> buf;

	buf.lock_back().get() = 1337;

	buf.swap();
	buf.get_front() == 1337 || TESTFAIL;
	buf.lock_back().get() = 8;

	buf.swap();
	buf.lock_back().get() == 1337 || TESTFAIL;
	buf.get_front() == 8 || TESTFAIL;
}

void concurrent_double_buffer_mt() {
	ConcurrentDoubleBuffer<int> buf(1, 2);

	// true if test succeeds, false otherwise
	std::atomic<bool> succ(true);
	std::atomic<int> sync(0);

	std::thread t1([&] {
			if (buf.get_front() != 1 || buf.lock_back().get() != 2) {
				succ.store(false);
			}

			sync.fetch_add(1);
			while (sync.load() != 2) {}

			buf.lock_back().get() = 3;

			sync.store(3);
		} );

	std::thread([&] {
			if (buf.lock_back().get() != 2 || buf.get_front() != 1) {
				succ.store(false);
			}

			sync.fetch_add(1);
			while (sync.load() != 2) {}

			if (buf.get_front() != 1) {
				succ.store(false);
			}

			while (sync.load() != 3) {}

			buf.swap();

			if (buf.get_front() != 3 || buf.lock_back().get() != 1) {
				succ.store(false);
			}
		} ).join();
	t1.join();

	succ.load() || TESTFAIL;
}

// exported test
void concurrent_double_buffer() {
	concurrent_double_buffer_ctr();
	concurrent_double_buffer_st();
	concurrent_double_buffer_mt();
}

}}} // openage::datastructure::tests
