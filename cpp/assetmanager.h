#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

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

protected:
	Texture *load_texture(const std::string &name);

private:
	util::Dir *root;

	std::unordered_map<std::string, Texture *> textures;
};

}

#endif
