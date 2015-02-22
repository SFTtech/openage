// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_BLOCK_ALLOCATOR_STACK_FIXED_H_
#define OPENAGE_UTIL_BLOCK_ALLOCATOR_STACK_FIXED_H_

#include <cstdlib>
#include <memory>
#include <vector>

#include "stack_internal.h"

namespace openage {
namespace util {
namespace _alloc {

/**
 * core of the fixed_stack_allocator
 */
template<class T>
class fixed_stack_allocator {
protected:
	struct deleter {
		void operator ()(T *del) const {
			delete[] del;
		}
	};

	//does actual release, decision done at compile time
	template<bool do_free>
	void releaser();

	using ptr_type = std::unique_ptr<T[], deleter>;
	ptr_type data;
	T *cur_ptr;
	T *cur_stackend;

public:
	fixed_stack_allocator(const fixed_stack_allocator &) = delete;
	fixed_stack_allocator &operator =(const fixed_stack_allocator &) = delete;

	fixed_stack_allocator(fixed_stack_allocator &&) = default;
	fixed_stack_allocator &operator =(fixed_stack_allocator &&) = default;
	~fixed_stack_allocator() = default;

	/**
	 * Constructs a stack allocator.
	 * Each substack contains stack_size elements,
	 * and there can only be stack_limit elements
	 * or unlimited if stack_limit==0
	 * @param stack_size The size of each sub-stack used in allocating
	 */
	fixed_stack_allocator(size_t stack_size);

	/**
	 * Retrieves a pointer to an location in memory that holds 1 T,
	 * except uninitialized. If allocations fails, return nullptr.
	 */
	T *new_ptr();

	/**
	 * Removes empty blocks (Has no effect on this class)
	 * provided for api symmetry with stack_allocator.
	 */
	inline void shrink_to_fit() {}

	/**
	 * Resets the allocator to starting state, may destruct
	 */
	template<bool destruct>
	void doclear();
};

template<class T>
fixed_stack_allocator<T>::fixed_stack_allocator(size_t stack_size)
	:
	data(new T[stack_size], deleter{}),
	cur_ptr(data.get()),
	cur_stackend(cur_ptr + stack_size) {
}

template<class T>
T *fixed_stack_allocator<T>::new_ptr() {
	if (likely(this->cur_ptr != this->cur_stackend)) {
		return this->cur_ptr++;
	}
	return nullptr;
}

template<class T>
template<bool do_release>
void fixed_stack_allocator<T>::releaser() {
	if (likely(this->cur_ptr != this->data.get())) {
		this->cur_ptr -= 1;
		if (do_release) {
			this->cur_ptr->~T();
		}
	}
}

template<class T>
template<bool destruct>
void fixed_stack_allocator<T>::doclear() {
	while (this->cur_ptr > this->data.get()) {
		this->cur_ptr -= 1;
		if (destruct) {
			this->cur_ptr->~T();
		}
	}
}

} // namespace _alloc


/**
 * This class emulates a stack, except in dynamic memory.
 * It only holds a single block of memory, and will
 * fail to allocate more once the limit has been reached.
 */
template<class T>
using fixed_stack_allocator = _alloc::stack_allocator_common<T, _alloc::fixed_stack_allocator>;

} // namespace util
} // namespace openage

#endif
