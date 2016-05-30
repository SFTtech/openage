// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_dedicated_thread.h"

#include <mutex>
#include <condition_variable>

#include <QCoreApplication>

#include "gui_application_impl.h"

namespace qtsdl {

std::weak_ptr<GuiDedicatedThread> GuiDedicatedThread::instance;

bool GuiDedicatedThread::exists = false;
std::mutex GuiDedicatedThread::existence_guard;
std::condition_variable GuiDedicatedThread::destroyed;

GuiDedicatedThread::GuiDedicatedThread()
	:
	worker{} {

	bool gui_started = false;
	std::mutex gui_started_guard;
	std::unique_lock<std::mutex> lck{gui_started_guard};

	std::condition_variable proceed_cond;

	this->worker = std::thread{[&] {
		auto app = GuiApplicationImpl::get();

		{
			std::unique_lock<std::mutex> lckInGui{gui_started_guard};
			gui_started = true;
		}

		proceed_cond.notify_one();

		QCoreApplication::instance()->exec();
	}};

	proceed_cond.wait(lck, [&] {return gui_started;});
}

GuiDedicatedThread::~GuiDedicatedThread() {
	QCoreApplication::instance()->quit();
	this->worker.join();
}

std::shared_ptr<GuiDedicatedThread> GuiDedicatedThread::get() {
	std::shared_ptr<GuiDedicatedThread> candidate;

	std::unique_lock<std::mutex> lck{GuiDedicatedThread::existence_guard};

	GuiDedicatedThread::destroyed.wait(lck, [&candidate] {
		return (candidate = GuiDedicatedThread::instance.lock()) || !GuiDedicatedThread::exists;
	});

	if (!candidate) {
		GuiDedicatedThread::instance = candidate = std::shared_ptr<GuiDedicatedThread>{new GuiDedicatedThread, [] (GuiDedicatedThread *p) {
			delete p;

			if (p) {
				std::unique_lock<std::mutex> dlck{GuiDedicatedThread::existence_guard};
				GuiDedicatedThread::exists = false;

				dlck.unlock();
				GuiDedicatedThread::destroyed.notify_all();
			}
		}};

		GuiDedicatedThread::exists = true;

		lck.unlock();
		GuiDedicatedThread::destroyed.notify_all();
	}

	return candidate;
}

} // namespace qtsdl
