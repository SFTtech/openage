// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <iostream>

#include "../../log.h"
#include "block_dynamic.h"
#include "block_fixed.h"
#include "stack_dynamic.h"
#include "stack_fixed.h"

namespace openage {
namespace util {
namespace tests {

template<class T, class index_type=uint32_t>
union val_ptr {
	struct {
		T placeholder;
		index_type block_index;
	} data;
	val_ptr *next;
};


struct test_deleter {
	test_deleter(int *v)
		:
		val(v) {}
	~test_deleter() {
		*val = 0;
	}

	int *val;
};

template<class T>
int block_allocator_size() {
	block_allocator<T> alloc(10);
	size_t test1 = (size_t)alloc.new_ptr();
	size_t test2 = (size_t)alloc.new_ptr();
	if ((test2 - test1) != alloc.alloc_size()) { return 0; }
	return -1;
}

int block_allocator_free_test() {
	block_allocator<val_ptr<size_t>> alloc(10);
	alloc.new_ptr();
	val_ptr<size_t> *holder = alloc.new_ptr();
	alloc.new_ptr();
	alloc.free(holder);
	if (holder != alloc.new_ptr()) { return 0; }
	if ((size_t)holder + 2*alloc.alloc_size() != (size_t)alloc.new_ptr()) {
		return 1;
	}
	return -1;
}

int block_allocator_block_test() {
	block_allocator<int> alloc(10);
	for (int i = 0; i < 8; i++) {
		alloc.create();
	}
	int *slptr = alloc.create();
	alloc.create();
	int *first_ptr2 = alloc.create();
	alloc.free(slptr);
	if ((size_t)alloc.create() - (size_t)first_ptr2 != alloc.alloc_size()) {
		return 0;
	}
	for (int i = 0; i < 8; i++) {
		alloc.create();
	}
	if (alloc.create() != slptr) { return 1; }
	return -1;
}

int block_allocator_test_deleter() {
	int v1 = 1;
	block_allocator<test_deleter> alloc(10);
	test_deleter *td1 = alloc.create(&v1);
	if (v1 != 1) { return 0; }
	alloc.release(td1);
	if (v1 != 1) { return 1; }

	td1 = alloc.create(&v1);
	alloc.free(td1);
	if (v1 != 0) { return 2; }

	return -1;
}

void block_allocator() {
	int ret;
	const char *testname;
	if ((ret = block_allocator_size<char>()) != -1) {
		testname = "allocation layout for small objects";
	}
	else if ((ret = block_allocator_size<char[27]>()) != -1) {
		testname = "allocation layout for large objects";
	}
	else if ((ret = block_allocator_free_test()) != -1) {
		testname = "allocation freeing algorithm";
	}
	else if ((ret = block_allocator_test_deleter()) != -1) {
		testname = "allocation deletion algorithm";
	}
	else {
		return;
	}
	log::err("%s failed at stage %d", testname, ret);
	throw "failed block allocator test";
}

int stack_allocator_size() {
	util::stack_allocator<char> alloc(10);
	char *fst = alloc.new_ptr();
	if (alloc.new_ptr() - fst != 1) { return 0; }
	return -1;
}

int stack_allocator_free() {
	util::stack_allocator<char> alloc(10);
	char *fst = alloc.new_ptr();
	alloc.free();
	if (fst != alloc.new_ptr()) { return 0; }
	return -1;
}

int stack_allocator_blocks() {
	util::stack_allocator<char> alloc(10);
	for (int i = 0; i < 9; i++) {
		alloc.create();
	}
	char *lptr = alloc.create();
	char *f1ptr = alloc.create();
	alloc.free();
	alloc.free();
	if (alloc.create() != lptr) { return 0; }
	if (alloc.create() != f1ptr) { return 1; }
	return -1;
}

void stack_allocator() {
	int ret;
	const char *testname;
	if ((ret = stack_allocator_size()) != -1) {
		testname = "stack allocation layout";
	}
	else if ((ret = stack_allocator_free()) != -1) {
		testname = "stack freeing";
	}
	else if ((ret = stack_allocator_blocks()) != -1) {
		testname = "stack block allocation";
	}
	else {
		return;
	}
	log::err("%s failed at stage %d", testname, ret);
	throw "failed stackallocator test";
}

template<class T>
int fixed_block_allocator_size() {
	fixed_block_allocator<T> alloc(10);
	size_t test1 = (size_t)alloc.new_ptr();
	size_t test2 = (size_t)alloc.new_ptr();
	if ((test2 - test1) != alloc.alloc_size()) { return 0; }
	return -1;
}

int fixed_block_allocator_free_test() {
	fixed_block_allocator<val_ptr<size_t>> alloc(10);
	alloc.new_ptr();
	val_ptr<size_t> *holder = alloc.new_ptr();
	alloc.new_ptr();
	alloc.free(holder);

	if (holder != alloc.new_ptr()) { return 0; }
	if ((size_t)holder + 2*alloc.alloc_size() != (size_t)alloc.new_ptr()) {
		return 1;
	}
	return -1;
}

int fixed_block_allocator_fixed_test() {
	fixed_block_allocator<size_t> alloc(10);
	for (size_t i = 0; i < 9; i++) {
		alloc.new_ptr();
	}
	if (!alloc.new_ptr()) { return 0; }
	if (alloc.new_ptr()) { return 1; }
	return -1;
}

int fixed_block_allocator_test_deleter() {
	int v1 = 1;
	fixed_block_allocator<test_deleter> alloc(10);
	test_deleter *td1 = alloc.create(&v1);
	if (v1 != 1) { return 0; }
	alloc.release(td1);
	if (v1 != 1) { return 1; }

	td1 = alloc.create(&v1);
	alloc.free(td1);
	if (v1 != 0) { return 2; }

	return -1;
}

void fixed_block_allocator() {
	int ret;
	const char *testname;

	if ((ret = fixed_block_allocator_size<char>()) != -1) {
		testname = "fixed block allocation layout failed for small data";
	}
	else if ((ret = fixed_block_allocator_size<char>()) != -1) {
		testname = "fixed block allocation layout failed for large data";
	}
	else if ((ret = fixed_block_allocator_free_test()) != -1) {
		testname = "fixed_block_allocator freeing failed";
	}
	else if ((ret = fixed_block_allocator_fixed_test()) != -1) {
		testname = "fixed_block_allocator is not fixed, failed";
	}
	else if ((ret = fixed_block_allocator_test_deleter()) != -1) {
		testname = "allocation deletion algorithm failed";
	}
	else {
		return;
	}

	log::err("%s at stage %d", testname, ret);
	throw "failed fixed_block_allocator test";
}

int fixed_stack_allocator_size() {
	util::fixed_stack_allocator<char> alloc(10);
	char *fst = alloc.new_ptr();
	if (alloc.new_ptr() - fst != 1) { return 0; }
	return -1;
}

int fixed_stack_allocator_free() {
	util::fixed_stack_allocator<char> alloc(10);
	char *fst = alloc.new_ptr();
	alloc.free();
	if (fst != alloc.new_ptr()) { return 0; }
	return -1;
}

int fixed_stack_allocator_fixed_test() {
	fixed_stack_allocator<size_t> alloc(10);
	for (size_t i = 0; i < 9; i++) {
		alloc.new_ptr();
	}
	if (!alloc.new_ptr()) { return 0; }
	if (alloc.new_ptr()) { return 1; }
	return -1;
}


void fixed_stack_allocator() {
	int ret;
	const char *testname;
	if ((ret = fixed_stack_allocator_size()) != -1) {
		testname = "stack allocation layout failed";
	}
	else if ((ret = fixed_stack_allocator_free()) != -1) {
		testname = "stack freeing failed";
	}
	else if ((ret = fixed_stack_allocator_fixed_test()) != -1) {
		testname = "fixed_block_allocator is not fixed";
	}
	else {
		return;
	}
	log::err("%s failed at stage %d", testname, ret);
	throw "failed fixed_stack_allocator test";

}


}
}
}
