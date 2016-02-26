// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace qtsdl {

/**
 * Runs the gui logic in separate thread.
 *
 * For sharing the thread with something else, use qtsdl::GuiApplicationImpl instead.
 */
class GuiDedicatedThread {
public:
	static std::shared_ptr<GuiDedicatedThread> get();

	~GuiDedicatedThread();

private:
	GuiDedicatedThread();

	std::thread worker;

	static std::weak_ptr<GuiDedicatedThread> instance;

	static bool exists;
	static std::mutex existence_guard;
	static std::condition_variable destroyed;
};

} // namespace qtsdl
