// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace openage {
namespace datastructure {

/** A threadsafe queue. */
template<typename T>
class ConcurrentQueue {
public:
	/** Removes all elements from the queue. */
	void clear() {
		std::lock_guard<std::mutex> lock{this->mutex};
		while (!this->queue.empty()) {
			this->queue.pop();
		}
	}

	/** Returns whether the queue is empty. */
	bool empty() {
		std::lock_guard<std::mutex> lock{this->mutex};
		return this->queue.empty();
	}

	/** Returns the front item of the queue without removing it. */
	T &front() {
		std::unique_lock<std::mutex> lock{this->mutex};
		while (this->queue.empty()) {
			this->elements_available.wait(lock);
		}
		return this->queue.front();
	}

	/** Removes the front item of the queue and returns it. */
	T &pop() {
		std::unique_lock<std::mutex> lock{this->mutex};
		while (this->queue.empty()) {
			this->elements_available.wait(lock);
		}
		auto &item = this->queue.front();
		this->queue.pop();
		return item;
	}

	/** Appends the given item to the queue. */
	void push(const T &item) {
		std::unique_lock<std::mutex> lock{this->mutex};
		this->queue.push(item);
		lock.unlock();
		this->elements_available.notify_one();
	}

private:
	/** The internally used queue. */
	std::queue<T> queue;

	/** The mutex to synchronize the queue. */
	std::mutex mutex;

	/**
	 * Condition variable to signal, whether elements are avaiable from the
	 * queue.
	 */
	std::condition_variable elements_available;
};

}
}
