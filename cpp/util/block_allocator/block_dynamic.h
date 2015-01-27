// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_BLOCK_ALLOCATOR_BLOCK_DYNAMIC_H_
#define OPENAGE_UTIL_BLOCK_ALLOCATOR_BLOCK_DYNAMIC_H_

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>

#include "block_internal.h"
#include "../compiler.h"
#include "../unique.h"

namespace openage {
namespace util {
namespace _alloc {

template<class T>
class block_allocator {
protected:
	using this_type = block_allocator<T>;
	struct _block;

	/** Holds the union of data and next free node. */
	union _val_ptr {
		struct {
			T placeholder;
			_block *holder;
		} data;
		_val_ptr *next;
	};

	/**
	 * deletion struct for unique_ptr
	 */
	struct internal_deleter {
		void operator ()(_val_ptr *del) const {
			free(del);
		}
	};

	/**
	 * struct that contains the memory being allocated
	 */
	struct _block {
		std::unique_ptr<_val_ptr[], internal_deleter> data;
		_val_ptr *first_open; //!< ptr to the first usable chunk
		size_t ptr_ind;       //!< index of this block
		size_t num_alloc;     //!< number of used chunks in this block
		_block(size_t data_len, size_t ind);
	};

	using ptr_type = std::unique_ptr<_block>;

	template<bool do_free>
	inline void releaser(T *to_release);

	void update_blocks(_block *block);

	/**
	 * create a new data block and add it to the lookup_vec
	 */
	void add_data();

	void swap_good();

	/** size of each block */
	size_t block_size;

	/**
	 * block lookup vector
	 * block_id -> allocated _block
	 */
	std::vector<ptr_type> lookup_vec;

	/**
	 * location of the last good data block
	 */
	size_t last_good;

public:

	block_allocator(const block_allocator &) = delete;
	block_allocator operator =(const block_allocator &) = delete;

	block_allocator(block_allocator &&) = default;
	block_allocator &operator =(block_allocator &&) = default;
	~block_allocator() = default;

	/**
	 * Constructs a block_allocator with a block_size of _block_size,
	 * limiting to block_limit. If block_limit <= 0 then an arbitrary
	 * amount of blocks will be allowed
	 * @param _block_size The size of each contiguous chunk of memory
	 * objects are taken from
	 */
	block_allocator(size_t _block_size);

	/**
	 * Retrieves a pointer to an location in memory that holds 1 T,
	 * except uninitialized. If allocations fails, return nullptr.
	 * @return A pointer pointing to an uninitialized T
	 */
	T *new_ptr();

	/** Deallocates empty blocks */
	void shrink_to_fit();

	/**
	 * Clears all blocks and deallocates all but 1, leaving the
	 * allocator in it's initial state.
	 */
	void deallocate();
};

template<class T>
block_allocator<T>::block_allocator(size_t _block_size)
	:
	block_size(_block_size) {
	this->add_data();
	this->last_good = 0;
}

template<class T>
void block_allocator<T>::add_data() {
	this->lookup_vec.emplace_back(
		util::make_unique<_block>(this->block_size, this->lookup_vec.size())
	);
	this->last_good += 1;
}

template<class T>
void  block_allocator<T>::swap_good() {
	std::swap(this->lookup_vec.back(),
	          this->lookup_vec[last_good]);

	std::swap(this->lookup_vec.back()->ptr_ind,
	          this->lookup_vec[last_good]->ptr_ind);

	this->last_good += 1;
}

/*
 * return a new data pointer, this is part of the first open block
 * and gets fetched from the chunks within that block.
 */
template<class T>
T *block_allocator<T>::new_ptr() {
	_block *block = lookup_vec.back().get();
	_val_ptr *fst = block->first_open;

	block->first_open = block->first_open->next;
	fst->data.holder = block;
	block->num_alloc += 1;

	if (unlikely(!block->first_open)) {
		if (last_good != lookup_vec.size()-1) {
			// swap the pointers to fill the gap of the previous
			// block freeing
			this->swap_good();
		}
		else {
			// a new block is needed.
			// allocate it and add it to the available list.
			this->add_data();
		}
	}

	return reinterpret_cast<T *>(fst);
}

template<class T>
void block_allocator<T>::update_blocks(_block *block) {
	size_t ind = block->ptr_ind;
	this->last_good -= 1;
	std::swap(this->lookup_vec[this->last_good], this->lookup_vec[ind]);
	this->lookup_vec[this->last_good]->ptr_ind = this->last_good;
	this->lookup_vec[ind]->ptr_ind = ind;
}

template<class T>
template<bool do_free>
inline void block_allocator<T>::releaser(T *to_release) {
	if (likely(to_release)) {
		_val_ptr *vptr = reinterpret_cast<_val_ptr *>(to_release);
		_block *block = vptr->data.holder;

		if (do_free) {
			to_release->~T();
		}
		if (unlikely(!block->first_open)) {
			this->update_blocks(block);
		}

		vptr->next = block->first_open;
		block->first_open = vptr;
		block->num_alloc -= 1;
	}
}

template<class T>
void block_allocator<T>::shrink_to_fit() {
	auto last_good = std::remove_if(
		this->lookup_vec.begin() + this->last_good + 1,
		this->lookup_vec.end(),
		[](const ptr_type &blk) {
			return (blk->num_alloc > 0);
		}
	);

	this->lookup_vec.erase(last_good, this->lookup_vec.end());
}

template<class T>
void block_allocator<T>::deallocate() {
	this->lookup_vec.resize(1);
	this->last_good = 0;
	auto &blkp = this->lookup_vec[0]->data;

	for (size_t i = 0; i < this->block_size-1; i++) {
		blkp[i].next = &blkp[i+1];
	}
	blkp[this->block_size-1].next = nullptr;

	this->lookup_vec[0]->num_alloc = 0;
	this->lookup_vec[0]->first_open = &blkp[0];
}

template<class T>
block_allocator<T>::_block::_block(size_t data_len, size_t ind)
	:
	data(static_cast<_val_ptr *>(malloc(data_len * sizeof(_val_ptr))),
	     internal_deleter{}),
	ptr_ind{ind} {

	// initialize all next-pointers
	for (size_t i = 0; i < data_len-1; i++) {
		this->data[i].next = &this->data[i+1];
	}
	this->data[data_len-1].next = nullptr;

	// the first usable data chunk in this block
	this->first_open = &this->data[0];
}

} // namespace _alloc


/**
 * This is a allocator for single allocations that returns memory out of
 * pre-allocated blocks instead of allocating memory on each call to
 * new. This makes it fairly cheap to allocate many small objects, and also
 * greatly improves cache coherency.
 *
 * Object allocated by this do not live longer than the duration of
 * the allocator. However, destructors must be called manually as with delete
 * if they perform meaningful work.
 *
 * When block_limit <= 0, the allocator will continuosly allocate more
 * blocks when needed. If block_limit > 0, then that will be the limit
 * set on the number of blocks. The maximum number of blocks is 2^32.
 */
template<class T>
using block_allocator = _alloc::block_allocator_common<T, _alloc::block_allocator>;

} // namespace util
} // namespace openage

#endif
