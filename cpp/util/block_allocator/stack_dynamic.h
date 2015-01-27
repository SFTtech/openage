// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_BLOCK_ALLOCATOR_STACK_DYNAMIC_H_
#define OPENAGE_UTIL_BLOCK_ALLOCATOR_STACK_DYNAMIC_H_

#include <cstdlib>
#include <memory>
#include <vector>

#include "stack_internal.h"
#include "../compiler.h"

namespace openage {
namespace util {
namespace _alloc {


/**
 * Implementation of a stack allocator.
 */
template<class T>
class stack_allocator {
protected:
	struct deleter {
		void operator ()(T *del) const {
			free(del);
		}
	};

	template<bool do_free>
	void releaser();

	template<bool do_free>
	void fancy_release();

	using ptr_type = std::unique_ptr<T[], deleter>;
	std::vector<ptr_type> ptrs;
	size_t stack_size;
	T *cur_ptr;
	T *cur_stackend;

	typename std::vector<ptr_type>::iterator cur_substack;

	void add_substack();

	void update_cur_stack();

public:
	stack_allocator(const stack_allocator &) = delete;
	stack_allocator &operator =(const stack_allocator &) = delete;

	stack_allocator(stack_allocator &&) = default;
	stack_allocator &operator =(stack_allocator &&) = default;
	~stack_allocator() = default;

	/**
	 * Constructs a stack allocator.
	 * Each substack contains stack_size elements,
	 * and there can only be stack_limit elements
	 * or unlimited if stack_limit==0
	 * @param stack_size The size of each sub-stack used in allocating
	 * @param stack_limit The limit n the number of sub stacks
	 */
	stack_allocator(size_t stack_size);

	/**
	 * Retrieves a pointer to an location in memory that holds 1 T,
	 * except uninitialized. If allocations fails, return nullptr.
	 * @return A pointer pointing to an uninitialized T
	 */
	T *new_ptr();

	/** Deallocates empty substacks */
	void shrink_to_fit();

	/** base implementation of the clearing function */
	template<bool destruct>
	void doclear();
};

template<class T>
stack_allocator<T>::stack_allocator(size_t _stack_size)
	:
	stack_size(_stack_size) {
	this->add_substack();
}

template<class T>
void stack_allocator<T>::add_substack() {
	T *space = static_cast<T *>(malloc(this->stack_size * sizeof(T)));

	this->ptrs.emplace_back(space, deleter{});
	this->cur_ptr = space;
	this->cur_stackend = space + this->stack_size;
	this->cur_substack = this->ptrs.end() - 1;
}

template<class T>
void stack_allocator<T>::update_cur_stack() {
	this->cur_substack += 1;
	if (this->cur_substack == ptrs.end()) {
		this->add_substack();
	}
	else {
		this->cur_ptr = this->cur_substack->get();
		this->cur_stackend = this->cur_ptr + this->stack_size;
	}
}

template<class T>
T *stack_allocator<T>::new_ptr() {
	if (unlikely(this->cur_ptr == this->cur_stackend)) {
		this->update_cur_stack();
	}
	return this->cur_ptr++;
}

template<class T>
template<bool do_free>
void stack_allocator<T>::fancy_release() {
	if (unlikely(this->cur_substack == this->ptrs.begin())) {
		return;
	}
	this->cur_substack -= 1;
	this->cur_stackend = this->cur_substack->get() + this->stack_size;
	this->cur_ptr = this->cur_stackend - 1;

	if (do_free) {
		this->cur_ptr->~T();
	}
}

template<class T>
template<bool do_free>
void stack_allocator<T>::releaser() {
	if (likely(this->cur_ptr != this->cur_substack->get())) {
		this->cur_ptr -= 1;
	}
	else {
		this->fancy_release<do_free>();
	}
	if (do_free) {
		this->cur_ptr->~T();
	}
}

template<class T>
template<bool destruct>
void stack_allocator<T>::doclear() {
	auto clearstack = [this]() {
		while (this->cur_ptr > this->cur_substack->get()) {
			this->cur_ptr -= 1;
			this->cur_ptr->~T();
		}
	};

	if (destruct) {
		clearstack();
		while (this->cur_substack != this->ptrs.begin()) {
			this->cur_substack -= 1;
			this->cur_ptr = *(this->cur_substack) + this->stack_size;
			clearstack();
		}
	}

	this->ptrs.resize(1);
	this->cur_ptr = this->ptrs[0].get();
	this->cur_substack = this->ptrs.begin();
}

template<class T>
void stack_allocator<T>::shrink_to_fit() {
	this->ptrs.erase(this->cur_substack + 1, this->ptrs.end());
	this->cur_substack = this->ptrs.end() - 1;
}

} // namespace _alloc

/**
 * This class emulates a stack, except in dynamic memory.
 * It will grow to accomodate the amount of memory needed,
 * and provides almost free allocations/deallocations.
 * However, memory can only be acquired/released from the top
 * of the stack.
 */
template<class T>
using stack_allocator = _alloc::stack_allocator_common<T, _alloc::stack_allocator>;

} // namespace util
} // namespace openage

#endif
