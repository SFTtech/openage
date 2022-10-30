// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <future>

#include <QObject>
#include <QStringList>

namespace qtgui {

/**
 * Emits a signal when anything changes in the directories.
 */
class RecursiveDirectoryWatcher : public QObject {
	Q_OBJECT

public:
	explicit RecursiveDirectoryWatcher(QObject *parent = nullptr);
	virtual ~RecursiveDirectoryWatcher();

signals:
	void changeDetected();
	void rootDirsPathsChanged(const QStringList &);
	void quit();

private:
	std::future<void> worker;
};

} // namespace qtgui
