// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <algorithm>
#include <memory>

#include "compiler.h"

namespace openage {
namespace util {


/**
 * A std::vector-like object that has a constexpr constructor, and thus can be
 * initialized during the const- or zero-initialization phase, before any
 * dynamic initialization happens.
 *
 * Use this class if and only if you need that functionality (that should
 * be pretty uncommon; there generally are better ways of guaranteeing dynamic
 * initialization order, such as static function variables).
 */
template<typename T>
class ConstInitVector {
public:
	constexpr ConstInitVector() noexcept : data{nullptr}, capacity{16}, count{0} {}


	~ConstInitVector() {
		std::allocator<T> alloc;

		if (this->data != nullptr) {
			for (size_t i = 0; i < this->count; i++) {
				alloc.destroy(&this->data[i]);
			}
			alloc.deallocate(this->data, this->capacity);
		}
	}


	/**
	 * Copying this is not supported.
	 */
	ConstInitVector(const ConstInitVector<T> &other) = delete;
	ConstInitVector(ConstInitVector<T> &&other) = delete;
	ConstInitVector &operator =(const ConstInitVector<T> &other) = delete;
	ConstInitVector &operator =(ConstInitVector<T> &&other) = delete;


	void push_back(const T &val) {
		std::allocator<T> alloc;

		if (unlikely(this->data == nullptr)) {
			// obj is fresh and needs to allocate memory.
			this->data = alloc.allocate(this->capacity);
		}

		if (unlikely(this->count == capacity)) {
			// obj is full and needs to resize memory.
			size_t newcapacity = capacity * 2;
			T *newdata = alloc.allocate(newcapacity);
			for (size_t i = 0; i < this->capacity; i++) {
				alloc.construct(&newdata[i], std::move_if_noexcept(this->data[i]));
				alloc.destroy(&this->data[i]);
			}
			alloc.deallocate(this->data, this->capacity);
			this->data = newdata;
			this->capacity = newcapacity;
		}

		// add val at the end.
		alloc.construct(&this->data[this->count], val);
		this->count += 1;
	}


	/**
	 * The returned reference is invalid if n >= this->size().
	 * It may be invalidated by a call to push_back().
	 */
	const T &operator[] (size_t idx) const {
		return this->data[idx];
	}


	/**
	 * Returns the number of elements that have been pushed back.
	 */
	size_t size() {
		return this->count;
	}

private:
	T *data;
	size_t capacity;
	size_t count;
};


}} // openage::util
