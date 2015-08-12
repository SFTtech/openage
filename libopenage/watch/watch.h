// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_WATCH_WATCH_H_
#define OPENAGE_WATCH_WATCH_H_

#include <functional>
#include <memory>
#include <unordered_set>

namespace openage {

/**
 * Filesystem watching functions.
 * Provides functionality for event based file monitoring.
 */
namespace watch {

/**
 * File watching event types.
 */
enum class event_type {
	create,   //!< new creation
	modify,   //!< content modification
	remove,   //!< removal of entry
};


/**
 * Function type for event callbacks.
 * has to be a function accepting (eventtype, filename)
 */
using callback_t = std::function<void(event_type, const std::string &)>;


/**
 * Handle for watching a single file.
 */
struct file_watcher {
	const callback_t callback;
	const std::string filename;
};


/**
 * File change and creation monitoring.
 */
class WatchManager {
protected:
	WatchManager() = default;

public:
	virtual ~WatchManager() = default;

	/**
	 * Check if any of the watched files/directories
	 * have any events queued and trigger the appropriate callback.
	 */
	virtual void check_changes() = 0;

	/**
	 * Create a new watch in this manager.
	 */
	virtual void watch_file(const std::string &filename, const callback_t &callback) = 0;

	/**
	 * Create a watch manager. Selects the plattform-compatible backend.
	 */
	static std::unique_ptr<WatchManager> create();
};

/**
 * dummy watch manager that is used when no watch functionality
 * has been compiled in.
 */
class DummyWatchManager : public WatchManager {
public:
	DummyWatchManager() = default;
	virtual ~DummyWatchManager() = default;

	/**
	 * Hehe, does just nothing.
	 */
	void check_changes() override;

	/**
	 * Huehue, does nothing as well.
	 */
	void watch_file(const std::string &, const callback_t &) override;
};


}} // namespace openage::watch

#endif
