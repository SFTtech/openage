// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <queue>
#include <cassert>

namespace openage {
namespace util {

/**
 * Abstract class for imitating python generators.
 */
template<class T>
class Generator {
private:
	std::queue<T> q;
protected:
	/**
	 * Called when the queue is empty and a value is requested.
	 * Use this->yield to fill the queue.
	 */
	virtual void generate() = 0;

	/**
	 * Push values to the internal queue.
	 */
	void yield(T data) {
		this->q.push(data);
	}
public:
	/**
	 * Generate next value.
	 */
	T operator ()() {
		T tmp;
		if (this->q.empty()) {
			this->generate();
		}
		tmp = this->q.front();
		this->q.pop();
		return tmp;
	}
};

}} // openage::util
