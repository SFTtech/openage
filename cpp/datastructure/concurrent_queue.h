#ifndef OPENAGE_DATASTUCTURE_CONCURRENT_QUEUE_H_
#define OPENAGE_DATASTUCTURE_CONCURRENT_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace openage {
namespace datastructure {

template<typename T>
class ConcurrentQueue {
public:
	void clear() {
		std::unique_lock<std::mutex> lock{mutex};
		while (!this->queue.empty()) {
			this->queue.pop();
		}
	}

	bool empty() {
		std::unique_lock<std::mutex> lock{mutex};
		return this->queue.empty();
	}

	T &front() {
		std::unique_lock<std::mutex> lock{mutex};
		while (this->queue.empty()) {
			this->elements_available.wait(lock);
		}
		return this->queue.front();
	}

	T &pop() {
		std::unique_lock<std::mutex> lock{mutex};
		while (this->queue.empty()) {
			this->elements_available.wait(lock);
		}
		auto &item = this->queue.front();
		this->queue.pop();
		return item;
	}

	void push(const T &item) {
		std::unique_lock<std::mutex> lock{mutex};
		this->queue.push(item);
		lock.unlock();
		this->elements_available.notify_one();
	}

private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable elements_available;
};

}
}

#endif
