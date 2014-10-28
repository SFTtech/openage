#include "assetmanager.h"

namespace openage {

AssetManager::AssetManager(util::Dir *_root):
		root(_root) {
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
	}else
		ret = new Texture{filename, true};

	textures[filename] = ret;

	return ret;
}

Texture *AssetManager::get_texture(const std::string &name) {
	if(textures.find(root->join(name)) == textures.end())
		return load_texture(name);

	return textures[root->join(name)];
}

}
