// Copyright 2017-2024 the openage authors. See copying.md for legal info.


#pragma once

#include <array>

#include "curve/base_curve.h"
#include "curve/keyframe_container.h"


// remember to update docs
namespace openage {
namespace curve {

template <typename T, size_t Size>
class Array {
public:
	using this_type = Array<T, Size>;
	using container_t = std::array<KeyframeContainer<T>, Size>;


	// Array() = default;

	// prevent accidental copy of queue
	// Array(const Array &) = delete;


	T get(const time::time_t &t, const size_t index) const;
	// T get(const time::time_t& t, const size_t index, const size_t hint) const;


	std::array<T, Size> get_all(const time::time_t &t) const;
	// std::array<Keyframe<T>, size> get_all(const time::time_t& t, const size_t hint) const;


	size_t size() const;

	const Keyframe<T> &frame(const time::time_t &t, const size_t index) const;


	const Keyframe<T> &next_frame(const time::time_t &t, const size_t index) const;

	void set_insert(const time::time_t &t, const size_t index, T value);

	void set_last(const time::time_t &t, const size_t index, T value);

	void set_replace(const time::time_t &t, const size_t index, T value);

	void sync(const Array<T, Size> &other, const time::time_t &t);


	class Iterator {
	public:
		const T *ptr;
		size_t offset;
		const Array<T, Size> *const curve;
		time::time_t time;

		Iterator(const Array<T, Size> *curve, const time::time_t &time = time::TIME_MAX, size_t offset = 0) :
			curve(curve), time(time), offset(offset) {
			if (this->offset != Size) {
				std::cout << this->offset << Size;
				ptr = &this->curve->frame(this->time, this->offset).value;
			}
		};

		const T &operator*() {
			ptr = &curve->frame(this->time, this->offset).value;
			return *this->ptr;
		}

		void operator++() {
			this->offset++;
		}

		bool operator!=(const Array<T, Size>::Iterator &rhs) const {
			return this->offset != rhs.offset;
		}
	};


	Iterator begin(const time::time_t &time = time::TIME_MAX) const;

	Iterator end(const time::time_t &time = time::TIME_MAX) const;


private:
	container_t container;
	mutable size_t last_hit_index = 0;
};


template <typename T, size_t Size>
const Keyframe<T> &Array<T, Size>::frame(const time::time_t &t, const size_t index) const {
	this->last_hit_index = container[index].last(t, this->last_hit_index);
	return container[index].get(this->last_hit_index);
}

template <typename T, size_t Size>
const Keyframe<T> &Array<T, Size>::next_frame(const time::time_t &t, const size_t index) const {
	this->last_hit_index = container[index].last(t, this->last_hit_index);
	return container[index].get(this->last_hit_index + 1);
}

template <typename T, size_t Size>
T Array<T, Size>::get(const time::time_t &t, const size_t index) const {
	return this->frame(t, index).value;
}

template <typename T, size_t Size>
std::array<T, Size> Array<T, Size>::get_all(const time::time_t &t) const {
	return [&]<auto... I>(std::index_sequence<I...>) {
		return std::array<T, Size>{this->get(t, I)...};
	}(std::make_index_sequence<Size>{});
}

template <typename T, size_t Size>
size_t Array<T, Size>::size() const {
	return Size;
}


template <typename T, size_t Size>
void Array<T, Size>::set_insert(const time::time_t &t, const size_t index, T value) {
	this->last_hit_index = this->container[index].insert_after(Keyframe(t, value), this->last_hit_index);
}


template <typename T, size_t Size>
void Array<T, Size>::set_last(const time::time_t &t, const size_t index, T value) {
	this->last_hit_index = this->container[index].insert_after(Keyframe(t, value), this->last_hit_index);
	this->container[index].erase_after(this->last_hit_index);
}


template <typename T, size_t Size>
void Array<T, Size>::set_replace(const time::time_t &t, const size_t index, T value) {
	this->container[index].insert_overwrite(Keyframe(t, value), this->last_hit_index);
}

template <typename T, size_t Size>
void Array<T, Size>::sync(const Array<T, Size> &other, const time::time_t &start) {
	for (int i = 0; i < Size; i++) {
		this->container[i].sync(other, start);
	}
}

template <typename T, size_t Size>
typename Array<T, Size>::Iterator Array<T, Size>::begin(const time::time_t &time) const {
	return Array<T, Size>::Iterator(this, time);
}


template <typename T, size_t Size>
typename Array<T, Size>::Iterator Array<T, Size>::end(const time::time_t &time) const {
	return Array<T, Size>::Iterator(this, time, this->container.size());
}

} // namespace curve
} // namespace openage
