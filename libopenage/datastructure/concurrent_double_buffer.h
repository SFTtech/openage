// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>


namespace openage {
namespace datastructure {

template<typename T>
class ConcurrentDoubleBuffer;

/// A lock that allows access to the data in the back buffer.
template<typename T>
class ConcurrentDoubleBufferLock {
public:
	/// Returns a reference to the back buffer data.
	T& get() {
		return *this->data;
	}

private:
	friend class ConcurrentDoubleBuffer<T>;
	ConcurrentDoubleBufferLock(std::unique_lock<std::mutex> &&lock, T* data)
		: lock(std::move(lock))
		, data(data) {}

	/// A lock on writing to the back and swapping the buffers.
	std::unique_lock<std::mutex> lock;

	/// The data in the back buffer.
	T* data;
};

/**
 * A single-reader multiple-writer double buffer structure. It has a front and back
 * buffer. The back is meant for writing, while the front should be read from.
 * The intended usage if for writers to store new information in the back
 * while the front is being read. After the reader is done, it locks the back
 * buffer (same as the writers do) and swaps the front with the back. If the data
 * doesn't differ by much, that might also involve copying the new front into
 * the new back to have resume writing from the most recent state.
 */
template<typename T>
class ConcurrentDoubleBuffer {
public:
	/// Tries to default-construct the buffers.
	ConcurrentDoubleBuffer() {}

	/// Copy-constructs the buffers.
	ConcurrentDoubleBuffer(const T &front, const T &back)
		: one(front)
		, two(back) {}

	/// Move-constructs the buffers.
	ConcurrentDoubleBuffer(T &&front, T &&back)
		: one(std::move(front))
		, two(std::move(back)) {}

	/// Returns a reference to the data in the front buffer.
	T& get_front() {
		if (one_is_front) {
			return this->one;
		}
		else {
			return this->two;
		}
	}

	/// Returns a writer lock on the back buffer.
	ConcurrentDoubleBufferLock<T> lock_back() {
		if (one_is_front) {
			return ConcurrentDoubleBufferLock<T>(std::unique_lock<std::mutex>(lock), &two);
		}
		else {
			return ConcurrentDoubleBufferLock<T>(std::unique_lock<std::mutex>(lock), &one);
		}
	}

	/// Swaps the buffers. It must be ensured that no concurrent readers are working.
	void swap() {
		std::unique_lock<std::mutex> lk(this->lock);
		this->one_is_front = !this->one_is_front;
	}

private:
	/// A lock on writing to the back and swapping the buffers.
	std::mutex lock;

	/// Which buffer is the front. If true, one is the front, else two is the front.
	bool one_is_front = true;

	/// The buffer data.
	T one, two;
};

}} // openage::datastructure
