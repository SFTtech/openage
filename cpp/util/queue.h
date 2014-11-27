// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_QUEUE_H_
#define OPENAGE_UTIL_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <deque>
#include <utility>
#include <cstddef>             // size_t
#include <thread>
#include <atomic>
#include <chrono>

namespace openage {
namespace util {

/**
 * Thread safe queue class
 */
template <typename T, typename Container=std::deque<T>>
class Queue {
private:
	std::mutex              mutex;
	std::condition_variable condition;
	Container           	container;
	std::atomic<bool>	aborted;
public:
	using container_type  = Container;
	using value_type      = typename container_type::value_type;
	using reference       = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	using size_type       = typename container_type::size_type;
	
	Queue() : aborted{false} {}
	Queue(size_t init_size) 
		: aborted{false} {
		this->queue.reserve(init_size);
	}
	~Queue() = default;
	
	// this class is a move-only type
	Queue(Queue const&) = delete;
	Queue& operator=(Queue const&) = delete;
	Queue(Queue &&) = default;
	Queue& operator=(Queue &&) = default;

	/**
	 * Push some value into this queue and notify one waiting thread.
	 */
	void push(value_type value) {
		std::unique_lock<std::mutex> lock(this->mutex);
		this->container.push_front(std::move(value));
		this->condition.notify_one();
		
	}

	/**
	 * Try to pop some value from this queue and (if needed) waits at most for the given duration.
	 * Returns true if the poping operation succeeded.
	 */
	template <typename Rep, typename ratio>
	bool pop(value_type& value, std::chrono::duration<Rep, ratio> const& duration) {
 		std::unique_lock<std::mutex> lock(this->mutex);
		if(!this->container.empty()) {
			value = std::move(this->container.back());
			this->container.pop_back();
			return true;
		}
		else if(!this->aborted) {
			this->condition.wait_for(lock, duration);
			if(!this->container.empty()) {
				value = std::move(this->container.back());
				this->container.pop_back();
				return true;
			}
			else {
				// spurious wake-up or this->abort() called from another thread
				return false;
			}
		}
		else {
			// this->abort() called from another thread
			return false;		
		}
	}
	
	/**
         * After a call to this->abort() any call to pop will not wait if the queue is not empty. Basically, any new call
	 * to this->pop() becomes synchronous.
	 * If any thread is currently waiting in this->pop() when this->abort() is called the user might want to call
	 * this->notify() to wake-up the waiting thread.
	 * To resume asynchronous operations, call this->resume().
	 */
	void abort() {
		this->aborted = true;
	}

	/**
	 * Notify one poping thread to wake up after a call to this->abort()
	 * Just to make everyone happy (and especially helgrind) do not call this->notify()
	 * if not necessary (i.e there's no waiting thread).
	 */
	void notify() {
		this->condition.notify_one();	
	}

	/**
	 * Initialy returns false. Return true after this->abort() has been called.
	 */
	std::atomic<bool> const& is_aborted() const {
		return this->aborted;
	}

	/**
	 * Resume asynchronous operations on this queue.
	 * this->is_aborted() returns false again after a call to this->resume()
	 */
	void resume() {
		this->aborted = false;
	}
};

} //namespace util
} //namespace openage

#endif

