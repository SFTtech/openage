#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include "config.h"

#include <unordered_map>
#include <string>

#include "texture.h"

namespace openage {

class AssetManager {
public:
	AssetManager(util::Dir *root);
	virtual ~AssetManager();

	bool can_load(const std::string &name) const;
	Texture *get_texture(const std::string &name);

	void check_updates();

protected:
	Texture *load_texture(const std::string &name);

private:
	util::Dir *root;

	std::unordered_map<std::string, Texture *> textures;

#if HAS_INOTIFY
	int notify_fd;
	std::unordered_map<int, Texture *> watch_fds;
#endif
};

}

#endif
