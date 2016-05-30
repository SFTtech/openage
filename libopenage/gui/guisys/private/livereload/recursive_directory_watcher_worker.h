// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QFileSystemWatcher>
#include <QStringList>
#include <QTimer>

namespace qtsdl {

/**
 * Emits a signal when anything changes in the directories.
 */
class RecursiveDirectoryWatcherWorker : public QObject {
	Q_OBJECT

public:
	RecursiveDirectoryWatcherWorker();

signals:
	void changeDetected();

public slots:
	void onRootDirsPathsChanged(const QStringList &root_dirs_paths);
	void restartWatching();

private slots:
	void onEntryChanged();

private:
	void restart_watching(const QStringList &entries_to_watch);

	/**
	 * Actual watcher.
	 * Its event processing and destruction has to be in the same separate thread.
	 */
	std::unique_ptr<QFileSystemWatcher> watcher;

	/**
	 * Waits to glue multiple changes into one event.
	 */
	QTimer batching_timer;

	QStringList root_dirs_paths;
};

} // namespace qtsdl
