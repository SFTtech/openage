// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "assetmanager.h"

#if WITH_INOTIFY
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h> /* for NAME_MAX */
#endif

#include "util/error.h"

namespace openage {

AssetManager::AssetManager(util::Dir *root)
	:
	root{root} {

	this->missing_tex = new Texture{root->join("missing.png"), false};

#if WITH_INOTIFY
	// initialize the inotify instance
	this->inotify_fd = inotify_init1(IN_NONBLOCK);
	if (this->inotify_fd < 0) {
		throw util::Error{"failed to initialize inotify!"};
	}
#endif
}

AssetManager::~AssetManager() {
	for (auto &tex : this->textures) {
		if (tex.second != this->missing_tex) {
			delete tex.second;
		}
	}
	delete this->missing_tex;
}

bool AssetManager::can_load(const std::string &name) const {
	return util::file_size(this->root->join(name)) > 0;
}

Texture *AssetManager::load_texture(const std::string &name) {
	std::string filename = this->root->join(name);

	Texture *ret;

	if (!this->can_load(name)) {
		log::msg("   file %s is not there...", filename.c_str());

		// TODO: add/fetch inotify watch on the containing folder
		// to display the tex as soon at it exists.

		// return the big X texture instead
		ret = this->missing_tex;
	} else {
		ret = new Texture{filename, true};

#if WITH_INOTIFY
		// create inotify update trigger for the requested file
		int wd = inotify_add_watch(this->inotify_fd, filename.c_str(), IN_CLOSE_WRITE);
		if (wd < 0) {
			throw util::Error{"failed to add inotify watch for %s", filename.c_str()};
		}
		this->watch_fds[wd] = ret;
#endif
	}

	this->textures[filename] = ret;

	return ret;
}

Texture *AssetManager::get_texture(const std::string &name) {
	auto tex_it = this->textures.find(this->root->join(name));

	// the texture was not loaded yet:
	if (tex_it == this->textures.end()) {
		return this->load_texture(name);
	}

	return tex_it->second;
}

void AssetManager::check_updates() {
#if WITH_INOTIFY
	// buffer for at least 4 inotify events
	char buf[4 * (sizeof(struct inotify_event) + NAME_MAX + 1)];
	ssize_t len;

	while (true) {
		// fetch all events, the kernel won't write "half" structs.
		len = read(this->inotify_fd, buf, sizeof(buf));

		if (len == -1 and errno == EAGAIN) {
			// no events, nothing to do.
			break;
		}
		else if (len == -1) {
			throw util::Error{"failed to read inotify events!"};
		}

		// process fetched events,
		// the kernel guarantees complete events in the buffer.
		char *ptr = buf;
		while (ptr < buf + len) {
			struct inotify_event *event = (struct inotify_event *)ptr;

			if (event->mask & IN_CLOSE_WRITE) {
				// TODO: this should invoke callback functions
				this->watch_fds[event->wd]->reload();
			}

			// move the buffer ptr to the next event.
			ptr += sizeof(struct inotify_event) + event->len;
		}
	}
#endif
}

}
