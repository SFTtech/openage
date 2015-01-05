// Copyright 2014-2015 the openage authors. See copying.md for legal info.

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
	root{root},
	missing_tex(nullptr, [](Texture *t) { delete t; }) {

#if WITH_INOTIFY
	// initialize the inotify instance
	this->inotify_fd = inotify_init1(IN_NONBLOCK);
	if (this->inotify_fd < 0) {
		throw util::Error{"failed to initialize inotify!"};
	}
#endif
}

bool AssetManager::can_load(const std::string &name) const {
	return util::file_size(this->root->join(name)) > 0;
}

Texture *AssetManager::load_texture(const std::string &name) {
	std::string filename = this->root->join(name);

	tex_ptr ret(nullptr, [](Texture *t) { delete t; });

	if (!this->can_load(name)) {
		log::msg("   file %s is not there...", filename.c_str());

		// TODO: add/fetch inotify watch on the containing folder
		// to display the tex as soon at it exists.

		// return the big X texture instead
		ret = get_missing_tex();
	} else {
		ret.reset(new Texture{filename, true});

#if WITH_INOTIFY
		// create inotify update trigger for the requested file
		int wd = inotify_add_watch(this->inotify_fd, filename.c_str(), IN_CLOSE_WRITE);
		if (wd < 0) {
			throw util::Error{"failed to add inotify watch for %s", filename.c_str()};
		}
		this->watch_fds[wd] = ret.get();
#endif
	}

	return textures.emplace(std::move(filename), std::move(ret)).first->second.get();
}

Texture *AssetManager::get_texture(const std::string &name) {
	auto tex_it = this->textures.find(this->root->join(name));

	// the texture was not loaded yet:
	if (tex_it == this->textures.end()) {
		return this->load_texture(name);
	}

	return tex_it->second.get();
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

AssetManager::tex_ptr AssetManager::get_missing_tex() {
	// Lazily load the missing texture.
	if (!missing_tex) {
		missing_tex.reset(new Texture{root->join("missing.png"), false});
	}
	
	// Create pseudo owning ptr with a dummy deleter so there are no double deletes.
	return tex_ptr(missing_tex.get(), [](Texture*) { });
}

}
