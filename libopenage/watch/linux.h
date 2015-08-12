// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_WATCH_LINUX_H_
#define OPENAGE_WATCH_LINUX_H_

#include "../config.h"
#if WITH_INOTIFY

#include <unordered_map>
#include <unordered_set>

#include "watch.h"


namespace openage {
namespace watch {
namespace linux {

/**
 * Number of events to fetch at once at maximum.
 */
constexpr size_t event_queue_size = 4;

/**
 * Linux file watcher powered by inotify.
 */
class WatchManager : public watch::WatchManager {
public:
	WatchManager();
	virtual ~WatchManager() = default;

	/**
	 * Query the inotify fds for events.
	 */
	void check_changes() override;

	/**
	 * Create a new watch in this manager.
	 */
	void watch_file(const std::string &filename, const callback_t &callback) override;


protected:
	/**
	 * Create an inotify control fd.
	 */
	int create_inotify_control();

	/**
	 * inotify control fd => watch handle fd => file watcher.
	 * We may have multiple fds open.
	 * The kernel returns the handle fd when events are triggered.
	 */
	std::unordered_map<int, std::unordered_map<int, file_watcher>> watch_fds;

	/**
	 * The number of currently opened inotify fds
	 *
	 * Global for the program!
	 */
	static size_t fd_count;

	/**
	 * Number of watches per control fd.
	 * "/proc/sys/fs/inotify/max_user_watches"
	 */
	static size_t max_user_watches;

	/**
	 * Number of opened inotify control fds.
	 * "/proc/sys/fs/inotify/max_user_instances"
	 */
	static size_t max_user_instances;

	/**
	 * Update the maximum inotify event counters from the /proc filesystem.
	 */
	static void fetch_limits();
};



}}} // namespace openage::watch::linux

#endif // WITH_INOTIFY
#endif
