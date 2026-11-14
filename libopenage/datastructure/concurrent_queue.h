// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

namespace openage::datastructure {
/**
 * A threadsafe queue.
 * Wraps the std::queue with a mutex.
 *
 * Supports waiting for data from another thread.
 */
template <typename T>
class ConcurrentQueue {
	/**
	 * Type of mutex used for the queue.
	 */
	using mutex_t = std::recursive_mutex;

public:
	/** Removes all elements from the queue. */
	void clear() {
		std::scoped_lock lock{this->mutex};
		while (!this->queue.empty()) {
			this->queue.pop();
		}
	}

	/** Returns whether the queue is empty. */
	bool empty() {
		std::scoped_lock lock{this->mutex};
		return this->queue.empty();
	}

	/** Returns the front item of the queue without removing it. */
	T &front() {
		std::unique_lock<mutex_t> lock{this->mutex};
		this->wait_for_elements(lock);
		return this->queue.front();
	}

	/** Copies the front item in the queue and removes it from the queue. */
	template <typename... None, typename U = T>
	T pop([[maybe_unused]] typename std::enable_if_t<!std::is_move_constructible_v<U> and std::is_copy_constructible_v<U>> *t = nullptr) {
		static_assert(sizeof...(None) == 0, "User-specified template arguments are prohibited.");
		std::unique_lock<mutex_t> lock{this->mutex};
		this->wait_for_elements(lock);
		T ret = this->queue.front();
		this->queue.pop();

		// Explicitly call the copy constructor since T cannot be move-constructed
		// TODO: change to `return ret;` when we migrate to GCC 10
		return T(ret);
	}

	/** Moves the front item in the queue and removes it from the queue. */
	template <typename... None, typename U = T>
	T pop([[maybe_unused]] typename std::enable_if_t<std::is_move_constructible_v<U>> *t = nullptr) {
		static_assert(sizeof...(None) == 0, "User-specified template arguments are prohibited.");
		std::unique_lock<mutex_t> lock{this->mutex};
		this->wait_for_elements(lock);
		T ret = std::move(this->queue.front());
		this->queue.pop();
		return ret;
	}

	/** Appends the given item to the queue by copying it. */
	template <typename... None, typename U = T>
	void push(typename std::enable_if_t<std::is_copy_constructible_v<U>, const T &> item) {
		static_assert(sizeof...(None) == 0, "User-specified template arguments are prohibited.");
		std::unique_lock<mutex_t> lock{this->mutex};
		this->queue.push(item);
		lock.unlock();
		this->elements_available.notify_one();
	}

	/** Appends the given item to the queue by moving it. */
	template <typename... None, typename U = T>
	void push(typename std::enable_if_t<std::is_move_constructible_v<U>, T &&> item) {
		static_assert(sizeof...(None) == 0, "User-specified template arguments are prohibited.");
		std::unique_lock<mutex_t> lock{this->mutex};
		this->queue.push(std::move(item));
		lock.unlock();
		this->elements_available.notify_one();
	}

	/**
	 * Return a lock to the queue so multiple
	 * of the above operations can be done sequentially
	 */
	std::unique_lock<mutex_t> lock() {
		return std::unique_lock<mutex_t>{this->mutex};
	}

private:
	/**
	 * Wait until the queue has elements.
	 *
	 * The wait only releases \p lock, so it must be the only lock
	 * this thread holds on the recursive mutex, otherwise producers
	 * can't push and the wait never ends.
	 */
	void wait_for_elements(std::unique_lock<mutex_t> &lock) {
		this->elements_available.wait(lock, [this] { return not this->queue.empty(); });
	}

	/** The internally used queue. */
	std::queue<T> queue;

	/** The mutex to synchronize the queue. */
	mutex_t mutex;

	/**
	 * Condition variable to signal, whether elements are avaiable from the
	 * queue.
	 */
	std::condition_variable_any elements_available;
};

} // namespace openage::datastructure
