// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "linux.h"

#if WITH_INOTIFY

#include <string>

// inotify headers
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>

#include "../util/file.h"

namespace openage {
namespace watch {
namespace linux {

size_t WatchManager::fd_count = 0;
size_t WatchManager::max_user_watches = 8192;
size_t WatchManager::max_user_instances = 128;


WatchManager::WatchManager() {
	// initialize the first inotify instance
	this->create_inotify_control();
}

int WatchManager::create_inotify_control() {
	// TODO: figure out the actual number.
	// other programs will be using inotify as well.
	if (this->fd_count == this->max_user_instances) {
		throw Error{MSG(err) << "number of inotify instances exhausted"};
	}

	int control_fd = inotify_init1(IN_NONBLOCK);
	if (control_fd < 0) {
		throw Error{MSG(err) << "Failed to initialize inotify!"};
	}

	this->watch_fds[control_fd];

	this->fd_count += 1;

	return control_fd;
}

// TODO: event type argument, don't hardcode CLOSE_WRITE.
void WatchManager::watch_file(const std::string &filename, const callback_t &callback) {

	// find the next free control fd slot.
	int cfd = -1;
	for (auto &it : this->watch_fds) {
		if (this->watch_fds[it.first].size() < this->max_user_watches) {
			cfd = it.first;
			break;
		}
	}

	if (cfd < 0) {
		cfd = this->create_inotify_control();
	}

	// create inotify update trigger for the requested file
	int wd = inotify_add_watch(cfd, filename.c_str(), IN_CLOSE_WRITE);
	if (wd < 0) {
		throw Error{MSG(warn) << "Failed to add inotify watch for " << filename};
	}
	this->watch_fds[cfd].emplace(std::make_pair(wd, file_watcher{callback, filename}));
}


void WatchManager::check_changes() {
	char buf[event_queue_size * (sizeof(struct inotify_event) + NAME_MAX + 1)];
	ssize_t len;

	for (auto &wfd : this->watch_fds) {
		int control_fd = wfd.first;

		while (true) {
			// fetch all events, the kernel won't write "half" structs.
			len = read(control_fd, buf, sizeof(buf));

			if (len == -1 and errno == EAGAIN) {
				// no events on this fd, nothing to do.
				break;
			}
			else if (len == -1) {
				throw Error{MSG(err) << "Failed to read inotify events!"};
			}

			// process fetched events.
			// the kernel guarantees complete events in the buffer.
			char *ptr = buf;
			while (ptr < buf + len) {
				auto *event = reinterpret_cast<struct inotify_event *>(ptr);

				// TODO: support more watch events
				if (event->mask & IN_CLOSE_WRITE) {
					// trigger the callback
					file_watcher &w = this->watch_fds[control_fd][event->wd];
					w.callback(event_type::modify, w.filename);
				}

				// move the buffer ptr to the next event.
				ptr += sizeof(struct inotify_event) + event->len;
			}
		}
	}
}


void WatchManager::fetch_limits() {
	WatchManager::max_user_watches = std::stoi(util::read_whole_file("/proc/sys/fs/inotify/max_user_watches"));

	WatchManager::max_user_instances = std::stoi(util::read_whole_file("/proc/sys/fs/inotify/max_user_instances"));
}


}}} // openage::watch::linux

#endif // WITH_INOTIFY
