// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "recursive_directory_watcher.h"

#include <QEventLoop>
#include <QSemaphore>

#include "renderer/gui/guisys/private/livereload/recursive_directory_watcher_worker.h"

namespace qtgui {

RecursiveDirectoryWatcher::RecursiveDirectoryWatcher(QObject *parent) :
	QObject{parent} {
	QSemaphore wait_worker_started;

	this->worker = std::async(std::launch::async, [this, &wait_worker_started] {
		QEventLoop loop;
		QObject::connect(this, &RecursiveDirectoryWatcher::quit, &loop, &QEventLoop::quit);

		RecursiveDirectoryWatcherWorker watcher;
		QObject::connect(&watcher, &RecursiveDirectoryWatcherWorker::changeDetected, this, &RecursiveDirectoryWatcher::changeDetected);
		QObject::connect(this, &RecursiveDirectoryWatcher::rootDirsPathsChanged, &watcher, &RecursiveDirectoryWatcherWorker::onRootDirsPathsChanged);

		wait_worker_started.release();

		loop.exec();
	});

	wait_worker_started.acquire();
}

RecursiveDirectoryWatcher::~RecursiveDirectoryWatcher() {
	emit this->quit();
	this->worker.wait();
}

} // namespace qtgui
