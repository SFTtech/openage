// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "recursive_directory_watcher_worker.h"

#include <algorithm>

#include <QDirIterator>
#include <QDebug>

namespace qtsdl {

namespace {
const int batch_ms = 100;
}

RecursiveDirectoryWatcherWorker::RecursiveDirectoryWatcherWorker()
	:
	QObject{} {

	this->batching_timer.setInterval(batch_ms);
	this->batching_timer.setSingleShot(true);
	QObject::connect(&this->batching_timer, &QTimer::timeout, this, &RecursiveDirectoryWatcherWorker::changeDetected);
	QObject::connect(&this->batching_timer, &QTimer::timeout, this, &RecursiveDirectoryWatcherWorker::restartWatching);
}

void RecursiveDirectoryWatcherWorker::onRootDirsPathsChanged(const QStringList &root_dirs_paths) {
	if (this->root_dirs_paths != root_dirs_paths) {
		this->root_dirs_paths = root_dirs_paths;
		this->restartWatching();
	}
}

namespace {
QStringList collect_entries_to_watch(const QStringList &root_dirs_paths) {
	QStringList root_dirs_paths_no_duplicates = root_dirs_paths;
	root_dirs_paths_no_duplicates.removeDuplicates();

	QStringList entries_to_watch;

	std::for_each(std::begin(root_dirs_paths_no_duplicates), std::end(root_dirs_paths_no_duplicates), [&entries_to_watch] (const QString& root_dir_path) {
		QDirIterator it{root_dir_path, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks};

		while (it.hasNext()) {
			entries_to_watch.append(it.next());
		}
	});

	return entries_to_watch;
}
}

void RecursiveDirectoryWatcherWorker::restartWatching() {
	this->restart_watching(collect_entries_to_watch(this->root_dirs_paths));
}

void RecursiveDirectoryWatcherWorker::restart_watching(const QStringList &entries_to_watch) {
	this->watcher.reset();
	this->watcher = std::move(std::make_unique<QFileSystemWatcher>());
	QObject::connect(&*this->watcher, &QFileSystemWatcher::directoryChanged, this, &RecursiveDirectoryWatcherWorker::onEntryChanged);

	if (entries_to_watch.empty())
		qWarning() << "RecursiveDirectoryWatcheWorker hasn't found any files to watch.";
	else
		this->watcher->addPaths(entries_to_watch);
}

void RecursiveDirectoryWatcherWorker::onEntryChanged() {
	if (!this->batching_timer.isActive())
		this->batching_timer.start();
}

} // namespace qtsdl
