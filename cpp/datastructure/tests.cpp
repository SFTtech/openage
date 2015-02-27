// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "tests.h"

#include "../log/log.h"
#include "../datastructure/doubly_linked_list.h"
#include "../datastructure/pairing_heap.h"


namespace openage {
namespace datastructure {
namespace tests {

int pairing_heap_0() {
	PairingHeap<int> heap{};
	int stage = 0;

	if (not (heap.size() == 0)) { return stage; }

	stage += 1; // 1:
	heap.push(0);
	heap.push(1);
	heap.push(2);
	heap.push(3);
	heap.push(4);

	// state: 01234
	stage += 1; // 2:
	if (not (heap.size() == 5)) { return stage; }
	if (not (heap.top() == 0)) { return stage; }

	stage += 1; // 3:
	if (not (0 == heap.pop())) { return stage; }
	if (not (1 == heap.pop())) { return stage; }
	if (not (2 == heap.pop())) { return stage; }
	if (not (3 == heap.pop())) { return stage; }

	stage += 1; // 4:
	if (not (heap.size() == 1)) { return stage; }

	stage += 1; // 5:
	heap.push(0);
	heap.push(10);

	// state: 0 4 10

	stage += 1; // 6:
	if (not (0 == heap.pop())) { return stage; }
	if (not (4 == heap.pop())) { return stage; }
	if (not (10 == heap.pop())) { return stage; }
	if (not (heap.size() == 0)) { return stage; }

	stage += 1; // 7:
	heap.push(5);
	heap.push(5);
	heap.push(0);
	heap.push(5);
	heap.push(5);
	if (not (0 == heap.pop())) { return stage; }
	if (not (5 == heap.pop())) { return stage; }
	if (not (5 == heap.pop())) { return stage; }
	if (not (5 == heap.pop())) { return stage; }
	if (not (5 == heap.pop())) { return stage; }

	if (not (heap.size() == 0)) { return stage; }
	return -1;
}


int pairing_heap_1() {
	int stage = 0;

	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{1});
	auto node = heap.push(heap_elem{2});
	heap.push(heap_elem{3});

	// 1 2 3
	stage += 1; // 1:
	node->data.data = 0;
	heap.update(node);

	// 0 1 3
	stage += 1; // 2:
	if (not (0 == heap.pop().data)) { return stage; }
	if (not (1 == heap.pop().data)) { return stage; }
	if (not (3 == heap.pop().data)) { return stage; }

	return -1;
}

int pairing_heap_2() {
	int stage = 0;

	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{1});
	auto node = heap.push(heap_elem{2});
	heap.push(heap_elem{3});

	// state: 1 2 3
	stage += 1; // 1:
	if (not (2 == heap.pop_node(node).data)) {return stage; }

	// state: 1 3
	stage += 1; // 2:
	if (not (1 == heap.pop().data)) { return stage; }
	if (not (3 == heap.pop().data)) { return stage; }

	return -1;
}

int pairing_heap_3() {
	int stage = 0;

	PairingHeap<heap_elem> heap{};
	heap.push(heap_elem{0});
	heap.push(heap_elem{1});
	heap.push(heap_elem{2});
	heap.push(heap_elem{3});
	heap.push(heap_elem{4});
	heap.push(heap_elem{5});
	heap.pop(); // trigger pairing

	heap.clear();

	stage += 1; // 1:
	if (heap.size() != 0) { return stage; }
	if (heap.empty() == false) { return stage; }

	return -1;
}


void pairing_heap() {
	int ret;
	const char *testname;
	if ((ret = pairing_heap_0()) != -1) {
		testname = "simple push pop test";
		goto out;
	}
	else if ((ret = pairing_heap_1()) != -1) {
		testname = "pairing heap update test";
		goto out;
	}
	else if ((ret = pairing_heap_2()) != -1) {
		testname = "pairing heap delete_node test";
		goto out;
	}
	else if ((ret = pairing_heap_3()) != -1) {
		testname = "pairing heap delete_node test";
		goto out;
	}

	return;
out:
	log::log(MSG(err) << testname << " failed at stage " << ret);
	throw "failed pairing heap tests";
}

void doubly_linked_list() {
	datastructure::DoublyLinkedList<int> list;
	int stage = 0;

	if (not list.empty()) { goto out; }

	list.push_front(0);
	list.push_front(1);
	list.push_front(2);

	list.push_back(3);
	list.push_front(4);
	list.push_back(5);
	stage += 1;

	// 421035
	if (not (list.size() == 6)) { goto out; }
	stage += 1;

	if (not (5 == list.pop_back()))  { goto out; }
	if (not (4 == list.pop_front())) { goto out; }
	stage += 1;

	// 2103
	if (not (list.size() == 4)) { goto out; }
	stage += 1;

	list.push_back(6);
	list.push_front(7);
	list.push_back(8);
	stage += 1;

	// 7210368
	if (not (8 == list.pop_back()))  { goto out; } stage += 1;
	if (not (6 == list.pop_back()))  { goto out; } stage += 1;
	if (not (3 == list.pop_back()))  { goto out; } stage += 1;
	if (not (7 == list.pop_front())) { goto out; } stage += 1;
	if (not (2 == list.pop_front())) { goto out; } stage += 1;
	if (not (0 == list.pop_back()))  { goto out; } stage += 1;
	if (not (1 == list.pop_back()))  { goto out; } stage += 1;

	if (not (list.size() == 0)) { goto out; }

	return;

out:
	log::log(MSG(err) << "linked list test failed at stage " << stage);
	throw "linked lisst test failed";
}

} // namespace tests
} // namespace datastructure
} // namespace openage
