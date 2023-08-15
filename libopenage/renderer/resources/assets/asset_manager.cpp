// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "asset_manager.h"

#include "error/error.h"
#include "log/log.h"
#include "log/message.h"

#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/assets/cache.h"
#include "renderer/resources/assets/texture_manager.h"
#include "renderer/resources/palette_info.h"
#include "renderer/resources/parser/parse_blendmask.h"
#include "renderer/resources/parser/parse_blendtable.h"
#include "renderer/resources/parser/parse_palette.h"
#include "renderer/resources/parser/parse_sprite.h"
#include "renderer/resources/parser/parse_terrain.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/terrain/blendpattern_info.h"
#include "renderer/resources/terrain/blendtable_info.h"
#include "renderer/resources/terrain/terrain_info.h"
#include "renderer/resources/texture_info.h"


namespace openage::renderer::resources {

AssetManager::AssetManager(const std::shared_ptr<Renderer> &renderer,
                           const util::Path &asset_base_dir) :
	renderer{renderer},
	cache{std::make_shared<AssetCache>()},
	texture_manager{std::make_shared<TextureManager>(renderer)},
	asset_base_dir{asset_base_dir} {
	log::log(INFO << "Created asset manager");
}

const std::shared_ptr<Animation2dInfo> &AssetManager::request_animation(const util::Path &path) {
	std::shared_ptr<Animation2dInfo> info;
	try {
		if (not this->cache->check_animation_cache(path)) {
			// create if not loaded
			info = std::make_shared<Animation2dInfo>(parser::parse_sprite_file(path, this->cache));
			this->cache->add_animation(path, info);
		}
	}
	catch (const Error &err) {
		if (this->placeholder_animation) {
			log::log(MSG(warn) << "Failed to load animation file from: " << path
			                   << " - using placeholder instead.");
			return (*this->placeholder_animation).second;
		}
		else {
			throw err;
		}
	}
	return this->cache->get_animation(path);
}

const std::shared_ptr<BlendPatternInfo> &AssetManager::request_blpattern(const util::Path &path) {
	std::shared_ptr<BlendPatternInfo> info;
	try {
		if (not this->cache->check_blpattern_cache(path)) {
			// create if not loaded
			info = std::make_shared<BlendPatternInfo>(parser::parse_blendmask_file(path, this->cache));
			this->cache->add_blpattern(path, info);
		}
	}
	catch (const Error &err) {
		if (this->placeholder_blpattern) {
			log::log(MSG(warn) << "Failed to load blend pattern file from: " << path
			                   << " - using placeholder instead.");
			return (*this->placeholder_blpattern).second;
		}
		else {
			throw err;
		}
	}
	return this->cache->get_blpattern(path);
}

const std::shared_ptr<BlendTableInfo> &AssetManager::request_bltable(const util::Path &path) {
	std::shared_ptr<BlendTableInfo> info;
	try {
		if (not this->cache->check_bltable_cache(path)) {
			// create if not loaded
			info = std::make_shared<BlendTableInfo>(parser::parse_blendtable_file(path, this->cache));
			this->cache->add_bltable(path, info);
		}
	}
	catch (const Error &err) {
		if (this->placeholder_bltable) {
			log::log(MSG(warn) << "Failed to load blend table file from: " << path
			                   << " - using placeholder instead.");
			return (*this->placeholder_bltable).second;
		}
		else {
			throw err;
		}
	}
	return this->cache->get_bltable(path);
}

const std::shared_ptr<PaletteInfo> &AssetManager::request_palette(const util::Path &path) {
	std::shared_ptr<PaletteInfo> info;
	try {
		if (not this->cache->check_palette_cache(path)) {
			// create if not loaded
			info = std::make_shared<PaletteInfo>(parser::parse_palette_file(path));
			this->cache->add_palette(path, info);
		}
	}
	catch (const Error &err) {
		if (this->placeholder_palette) {
			log::log(MSG(warn) << "Failed to load palette file from: " << path
			                   << " - using placeholder instead.");
			return (*this->placeholder_palette).second;
		}
		else {
			throw err;
		}
	}
	return this->cache->get_palette(path);
}

const std::shared_ptr<TerrainInfo> &AssetManager::request_terrain(const util::Path &path) {
	std::shared_ptr<TerrainInfo> info;
	try {
		if (not this->cache->check_terrain_cache(path)) {
			// create if not loaded
			info = std::make_shared<TerrainInfo>(parser::parse_terrain_file(path, this->cache));
			this->cache->add_terrain(path, info);
		}
	}
	catch (const Error &err) {
		if (this->placeholder_terrain) {
			log::log(MSG(warn) << "Failed to load terrain file from: " << path
			                   << " - using placeholder instead.");
			return (*this->placeholder_terrain).second;
		}
		else {
			throw err;
		}
	}
	return this->cache->get_terrain(path);
}

const std::shared_ptr<Texture2dInfo> &AssetManager::request_texture(const util::Path &path) {
	std::shared_ptr<Texture2dInfo> info;
	try {
		if (not this->cache->check_texture_cache(path)) {
			// create if not loaded
			info = std::make_shared<Texture2dInfo>(parser::parse_texture_file(path));
			this->cache->add_texture(path, info);
		}
	}
	catch (const Error &err) {
		if (this->placeholder_texture) {
			log::log(MSG(warn) << "Failed to load texture file from: " << path
			                   << " - using placeholder instead.");
			return (*this->placeholder_texture).second;
		}
		else {
			throw err;
		}
	}
	return this->cache->get_texture(path);
}

const std::shared_ptr<Animation2dInfo> &AssetManager::request_animation(const std::string &rel_path) {
	return this->request_animation(this->asset_base_dir / rel_path);
}

const std::shared_ptr<BlendPatternInfo> &AssetManager::request_blpattern(const std::string &rel_path) {
	return this->request_blpattern(this->asset_base_dir / rel_path);
}

const std::shared_ptr<BlendTableInfo> &AssetManager::request_bltable(const std::string &rel_path) {
	return this->request_bltable(this->asset_base_dir / rel_path);
}

const std::shared_ptr<PaletteInfo> &AssetManager::request_palette(const std::string &rel_path) {
	return this->request_palette(this->asset_base_dir / rel_path);
}

const std::shared_ptr<TerrainInfo> &AssetManager::request_terrain(const std::string &rel_path) {
	return this->request_terrain(this->asset_base_dir / rel_path);
}

const std::shared_ptr<Texture2dInfo> &AssetManager::request_texture(const std::string &rel_path) {
	return this->request_texture(this->asset_base_dir / rel_path);
}

void AssetManager::set_placeholder_animation(const util::Path &path) {
	this->placeholder_animation = std::make_pair(
		path,
		std::make_shared<Animation2dInfo>(
			parser::parse_sprite_file(path, this->cache)));
}

void AssetManager::set_placeholder_blpattern(const util::Path &path) {
	this->placeholder_blpattern = std::make_pair(
		path,
		std::make_shared<BlendPatternInfo>(
			parser::parse_blendmask_file(path, this->cache)));
}

void AssetManager::set_placeholder_bltable(const util::Path &path) {
	this->placeholder_bltable = std::make_pair(
		path,
		std::make_shared<BlendTableInfo>(
			parser::parse_blendtable_file(path, this->cache)));
}

void AssetManager::set_placeholder_palette(const util::Path &path) {
	this->placeholder_palette = std::make_pair(
		path,
		std::make_shared<PaletteInfo>(
			parser::parse_palette_file(path)));
}

void AssetManager::set_placeholder_terrain(const util::Path &path) {
	this->placeholder_terrain = std::make_pair(
		path,
		std::make_shared<TerrainInfo>(
			parser::parse_terrain_file(path, this->cache)));
}

void AssetManager::set_placeholder_texture(const util::Path &path) {
	this->placeholder_texture = std::make_pair(
		path,
		std::make_shared<Texture2dInfo>(
			parser::parse_texture_file(path)));
}

const AssetManager::placeholder_anim_t &AssetManager::get_placeholder_animation() {
	return this->placeholder_animation;
}

const AssetManager::placeholder_blpattern_t &AssetManager::get_placeholder_blpattern() {
	return this->placeholder_blpattern;
}

const AssetManager::placeholder_bltable_t &AssetManager::get_placeholder_bltable() {
	return this->placeholder_bltable;
}

const AssetManager::placeholder_palette_t &AssetManager::get_placeholder_palette() {
	return this->placeholder_palette;
}

const AssetManager::placeholder_terrain_t &AssetManager::get_placeholder_terrain() {
	return this->placeholder_terrain;
}

const AssetManager::placeholder_texture_t &AssetManager::get_placeholder_texture() {
	return this->placeholder_texture;
}

const std::shared_ptr<TextureManager> &AssetManager::get_texture_manager() {
	return this->texture_manager;
}

} // namespace openage::renderer::resources
