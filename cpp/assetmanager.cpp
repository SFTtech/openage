#include "assetmanager.h"

#if HAS_INOTIFY
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

#if HAS_INOTIFY
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

#if HAS_INOTIFY
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
	if(textures.find(root->join(name)) == textures.end())
		return load_texture(name);

	return textures[root->join(name)];
}

void AssetManager::check_updates() {
#if HAS_INOTIFY
	char buf[sizeof(struct inotify_event) + NAME_MAX + 1];
	ssize_t readed;

	while((readed = read(notify_fd, buf, sizeof(buf))) > 0){
		struct inotify_event *evt = (struct inotify_event *)buf;

		if(evt->mask & IN_CLOSE_WRITE)
			watch_fds[evt->wd]->reload();
	}
#endif
}

}
