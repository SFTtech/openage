// Copyright 2014-2023 the openage authors. See copying.md for legal info.

/**
 * TODO: Deprecated in favor of presenter/assets/asset_manager.h
 *
 */

#include "legacy_assetmanager.h"

#if WITH_INOTIFY
#include <climits> /* for NAME_MAX */
#include <sys/inotify.h>
#include <unistd.h>
#endif

#include "error/error.h"
#include "log/log.h"
#include "util/compiler.h"
#include "util/file.h"

#include "texture.h"

namespace openage {

LegacyAssetManager::LegacyAssetManager(qtsdl::GuiItemLink *gui_link) :
	missing_tex{nullptr},
	gui_link{gui_link} {
#if WITH_INOTIFY
	// initialize the inotify instance
	this->inotify_fd = inotify_init1(IN_NONBLOCK);
	if (this->inotify_fd < 0) {
		throw Error{MSG(err) << "Failed to initialize inotify!"};
	}
#endif
}


const util::Path &LegacyAssetManager::get_asset_dir() {
	return this->asset_path;
}


void LegacyAssetManager::set_asset_dir(const util::Path &new_path) {
	if (this->asset_path != new_path) {
		this->asset_path = new_path;
		this->clear();
	}
}


void LegacyAssetManager::set_display(presenter::LegacyDisplay *display) {
	this->display = display;
}


presenter::LegacyDisplay *LegacyAssetManager::get_display() const {
	return this->display;
}

void LegacyAssetManager::set_engine(LegacyEngine *engine) {
	this->engine = engine;
}

LegacyEngine *LegacyAssetManager::get_engine() const {
	return this->engine;
}


std::shared_ptr<Texture> LegacyAssetManager::load_texture(const std::string &name,
                                                          bool use_metafile,
                                                          bool null_if_missing) {
	// the texture to be associated with the given filename
	std::shared_ptr<Texture> tex;

	util::Path tex_path = this->asset_path[name];

	// try to open the texture filename.
	if (not tex_path.is_file()) {
		// TODO: add/fetch inotify watch on the containing folder
		// to display the tex as soon at it exists.

		if (null_if_missing) {
			return nullptr;
		}
		else {
			// return the big X texture instead
			tex = this->get_missing_tex();
		}
	}
	else {
		// create the texture!
		tex = std::make_shared<Texture>(tex_path, use_metafile);

#if WITH_INOTIFY
		std::string native_path = tex_path.resolve_native_path();

		if (native_path.size() > 0) {
			// create inotify update trigger for the requested file

			// TODO: let util::Path do the file watching
			int wd = inotify_add_watch(
				this->inotify_fd,
				native_path.c_str(),
				IN_CLOSE_WRITE);

			if (wd < 0) {
				log::log(WARN << "Failed to add inotify watch for " << native_path);
			}
			else {
				this->watch_fds[wd] = tex;
			}
		}
#endif
	}

	// pass back the shared_ptr<Texture>
	return tex;
}


Texture *LegacyAssetManager::get_texture(const std::string &name, bool use_metafile, bool null_if_missing) {
	// check whether the requested texture was loaded already
	auto tex_it = this->textures.find(name);

	// the texture was not loaded yet:
	if (tex_it == this->textures.end()) {
		auto tex = this->load_texture(name, use_metafile, null_if_missing);

		if (tex.get() != nullptr) {
			// insert the texture into the map
			this->textures.insert(std::make_pair(name, tex));
		}

		//  and return the texture pointer.
		return tex.get();
	}

	return tex_it->second.get();
}


void LegacyAssetManager::check_updates() {
#if WITH_INOTIFY
	// buffer for at least 4 inotify events
	char buf[4 * (sizeof(struct inotify_event) + NAME_MAX + 1)];
	ssize_t len;

	while (true) {
		// fetch all events, the kernel won't write "half" structs.
		len = read(this->inotify_fd, buf, sizeof(buf));

		if (len == -1) {
			if (errno == EAGAIN) {
				// no events, nothing to do.
				break;
			}
			else {
				// something went wrong
				log::log(WARN << "Failed to read inotify events!");
				break;
			}
		}

		// process fetched events,
		// the kernel guarantees complete events in the buffer.
		char *ptr = buf;
		while (ptr < buf + len) {
			auto *event = reinterpret_cast<struct inotify_event *>(ptr);

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

std::shared_ptr<Texture> LegacyAssetManager::get_missing_tex() {
	// if not loaded, fetch the "missing" texture (big red X).
	if (this->missing_tex.get() == nullptr) [[unlikely]] {
		this->missing_tex = std::make_shared<Texture>(
			this->asset_path["test"]["textures"]["missing.png"],
			false);
	}

	return this->missing_tex;
}

void LegacyAssetManager::clear() {
#if WITH_INOTIFY
	for (auto &watch_fd : this->watch_fds) {
		int result = inotify_rm_watch(this->inotify_fd, watch_fd.first);
		if (result < 0) {
			log::log(WARN << "Failed to remove inotify watches");
		}
	}
	this->watch_fds.clear();
#endif

	this->textures.clear();
}

} // namespace openage
