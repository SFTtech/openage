// Copyright 2015-2017 the openage authors. See copying.md for legal info.
#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace openage {
namespace datastructure {

template <class T>
class TaskQueue {
 public:
	template <class Func, class Cont>
	TaskQueue(Cont &outputVec,
	          Func func,
	          unsigned int numThreads = std::thread::hardware_concurrency()) {
		for (unsigned int i = 0; i < numThreads; ++i) {
			threads.emplace_back(
			    static_cast<void (TaskQueue::*)(Func, Cont &)>(&TaskQueue::process),
			    this, func, std::ref(outputVec));
		}
	}

	~TaskQueue() {
		done = true;
		cond.notify_all();
		for (auto &thread : threads) {
			thread.join();
		}
	}

	void addToQueue(const T& val) {
		std::lock_guard<std::mutex> lock(queueLock);
		loadQueue.push(val);
		cond.notify_one();
	}

	void addToQueue(T&& val) {
		std::lock_guard<std::mutex> lock(queueLock);
		loadQueue.push(std::move(val));
		cond.notify_one();
	}

 private:
	std::queue<T> loadQueue;

	std::mutex queueLock;
	std::mutex outputLock;

	std::vector<std::thread> threads;
	std::condition_variable cond;
	std::atomic<bool> done{false};

	template <class Func, class Cont>
	void process(Func func, Cont &outputContainer) {
		while (true) {
			typename std::queue<T>::value_type name;
			{
				std::unique_lock<std::mutex> lock(queueLock);
				cond.wait(lock, [this] { return !loadQueue.empty() || done; });
				if (loadQueue.size() > 0) {
					name = loadQueue.front();
					loadQueue.pop();
					lock.unlock();
				} else if (done) {
					break;
				}
			}
			const auto surface = func(name);
			{
				std::lock_guard<std::mutex> lock(outputLock);
				outputContainer.insert(outputContainer.end(), surface);
			}
		}
	}
};
}
}
