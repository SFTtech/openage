#include "../log.h"
#include "../datastructure/doubly_linked_list.h"
//#include "../datastructure/pairing_heap.h"

using namespace openage;

namespace testing {
namespace tests {

bool pairing_heap(int /*unused*/, char ** /*unused*/) {
	return false;
}

bool doubly_linked_list(int /*unused*/, char ** /*unused*/) {
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

	return true;

out:
	log::err("linked list test failed at stage %d", stage);
	return false;
}

} //namespace tests
} //namespace testing
