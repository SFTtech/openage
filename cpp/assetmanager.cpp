#include "assetmanager.h"

#if HAS_INOTIFY
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h> /* for NAME_MAX */
#endif

namespace openage {

AssetManager::AssetManager(util::Dir *_root):
		root(_root) {
#if HAS_INOTIFY
	notify_fd = inotify_init1(IN_NONBLOCK);
#endif
}

AssetManager::~AssetManager() {
	for(auto it : textures)
		delete it.second;
}

bool AssetManager::can_load(const std::string &name) const {
	return util::file_size(root->join(name)) > 0;
}

Texture *AssetManager::load_texture(const std::string &name) {
	std::string filename = root->join(name);

	Texture *ret;

	if(!can_load(name)){
		log::msg("   file %s is not there...", filename.c_str());
		ret = new Texture{root->join("missing.png"), false};
	}else{
		ret = new Texture{filename, true};

#if HAS_INOTIFY
		int wd = inotify_add_watch(notify_fd, filename.c_str(), IN_CLOSE_WRITE);
		watch_fds[wd] = ret;
#endif
	}

	textures[filename] = ret;

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
