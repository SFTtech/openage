// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_BLOCK_ALLOCATOR_BLOCK_FIXED_H_
#define OPENAGE_UTIL_BLOCK_ALLOCATOR_BLOCK_FIXED_H_

#include <algorithm>
#include <cstdlib>
#include <memory>

#include "block_internal.h"
#include "../compiler.h"

namespace openage {
namespace util {
namespace _alloc {


template<class T>
class fixed_block_allocator {
protected:
	using this_type = fixed_block_allocator<T>;

	/**
	 * Holds the data and next free node
	 */
	union _val_ptr {
		T placeholder;   //!< stored object
		_val_ptr *next;  //!< next object
	};

	/**
	 * deletion struct for unique_ptr
	 */
	struct internal_deleter {
		void operator ()(_val_ptr *del) const {
			free(del);
		}
	};

	template<bool do_free>
	void releaser(T *to_release);

	/** Attempts to retrieve a pointer, returns 0 otherwise. */
	T *_new_ptr();

	/** size of each block */
	size_t block_size;

	/** the type of the pointer the allocated data block */
	using ptr_type = std::unique_ptr<_val_ptr[], internal_deleter>;

	/** A pointer to the data being held */
	ptr_type data;

	/** A pointer to the first open value */
	_val_ptr *first_open;

public:
	fixed_block_allocator(const fixed_block_allocator &) = delete;
	fixed_block_allocator operator =(const fixed_block_allocator &) = delete;

	fixed_block_allocator(fixed_block_allocator &&) = default;
	fixed_block_allocator &operator =(fixed_block_allocator &&) = default;
	~fixed_block_allocator() = default;

	/**
	 * Constructs a fixed_block_allocator with a block_size of _block_size,
	 * @param _block_size The numer of objects in the block
	 */
	fixed_block_allocator(size_t _block_size);

	/**
	 * Retrieves a pointer to an location in memory that holds 1 T,
	 * except uninitialized. If allocations fails, return nullptr.
	 * @return A pointer pointing to an uninitialized T
	 */
	T *new_ptr();

	/**
	 * Removes empty blocks (has no effect on this class).
	 * provided for api symmetry with block_allocator.
	 */
	inline void shrink_to_fit() {}

	/** Resets the allocator to starting state, does not call destructors */
	void deallocate();
};


/*
 * TODO: replace malloc my new, but this calls default constructor.
 */
template<class T>
fixed_block_allocator<T>::fixed_block_allocator(size_t _block_size)
	:
	block_size(_block_size),
	data(static_cast<_val_ptr *>(malloc(this->block_size * sizeof(_val_ptr))),
	     internal_deleter{}),
	first_open(data.get()) {

	// Initialize all next-block pointers
	for (size_t i = 0; i < this->block_size-1; i++) {
		this->data[i].next = &this->data[i+1];
	}
	this->data[this->block_size-1].next = nullptr;
}

template<class T>
T *fixed_block_allocator<T>::new_ptr()  {
	// reverse since new blocks are pushed onto the back
	if (likely(this->first_open)) {
		_val_ptr *fst = this->first_open;
		this->first_open = this->first_open->next;
		return reinterpret_cast<T *>(fst);
	}
	return nullptr;
}

template<class T>
template<bool do_free>
void fixed_block_allocator<T>::releaser(T *to_release) {
	if (likely(to_release)) {
		_val_ptr *vptr = reinterpret_cast<_val_ptr *>(to_release);
		if (do_free) {
			to_release->~T();
		}
		vptr->next = this->first_open;
		this->first_open = vptr;
	}
}

template<class T>
void fixed_block_allocator<T>::deallocate() {
	for (size_t i = 0; i < this->block_size - 1; i++) {
		this->data[i]->next = &this->data[i+1];
	}
	this->data[this->block_size-1]->next = nullptr;
	this->first_open = &this->data;
}

} // namespace _alloc


/**
 * This is a allocator for single allocations that returns memory out of a
 * pre-allocated blocks instead of allocating memory on each call to
 * new. This makes it fairly cheap to allocate many small objects, and also
 * greatly improves memory locality.
 *
 * The fixed block alocator will only allocate out of a isngle block,
 * as opposed to a standard block allocator. This improves performance
 * and memory usage/locality, but strictly enforces limited memory usage
 *
 * Object allocated by this do not live longer than the duration of
 * the allocator. However, destructors must be called manually as with delete
 * if they perform meaningful work.
 */
template<class T>
using fixed_block_allocator = _alloc::block_allocator_common<T, _alloc::fixed_block_allocator>;

} // namespace util
} // namespace openage

#endif
