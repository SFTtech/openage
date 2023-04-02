// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "asset_manager.h"

#include "renderer/renderer.h"
#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/assets/cache.h"
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
#include "renderer/resources/texture_data.h"
#include "renderer/resources/texture_info.h"

namespace openage::renderer::resources {

AssetManager::AssetManager(const std::shared_ptr<Renderer> &renderer) :
	renderer{renderer},
	cache{std::make_shared<AssetCache>()},
	texture_manager{renderer} {
}

const std::shared_ptr<Animation2dInfo> &AssetManager::request_animation(const util::Path &path) {
	if (not this->cache->check_animation_cache(path)) {
		// create if not loaded
		auto info = std::make_shared<Animation2dInfo>(parser::parse_sprite_file(path, this->cache));
		this->cache->add_animation(path, info);
	}
	return this->cache->get_animation(path);
}

const std::shared_ptr<BlendPatternInfo> &AssetManager::request_blpattern(const util::Path &path) {
	if (not this->cache->check_blpattern_cache(path)) {
		// create if not loaded
		auto info = std::make_shared<BlendPatternInfo>(parser::parse_blendmask_file(path, this->cache));
		this->cache->add_blpattern(path, info);
	}
	return this->cache->get_blpattern(path);
}

const std::shared_ptr<BlendTableInfo> &AssetManager::request_bltable(const util::Path &path) {
	if (not this->cache->check_bltable_cache(path)) {
		// create if not loaded
		auto info = std::make_shared<BlendTableInfo>(parser::parse_blendtable_file(path, this->cache));
		this->cache->add_bltable(path, info);
	}
	return this->cache->get_bltable(path);
}

const std::shared_ptr<PaletteInfo> &AssetManager::request_palette(const util::Path &path) {
	if (not this->cache->check_palette_cache(path)) {
		// create if not loaded
		auto info = std::make_shared<PaletteInfo>(parser::parse_palette_file(path));
		this->cache->add_palette(path, info);
	}
	return this->cache->get_palette(path);
}

const std::shared_ptr<TerrainInfo> &AssetManager::request_terrain(const util::Path &path) {
	if (not this->cache->check_terrain_cache(path)) {
		// create if not loaded
		auto info = std::make_shared<TerrainInfo>(parser::parse_terrain_file(path, this->cache));
		this->cache->add_terrain(path, info);
	}
	return this->cache->get_terrain(path);
}

const std::shared_ptr<Texture2dInfo> &AssetManager::request_texture(const util::Path &path) {
	if (not this->cache->check_texture_cache(path)) {
		// create if not loaded
		auto info = std::make_shared<Texture2dInfo>(parser::parse_texture_file(path));
		this->cache->add_texture(path, info);
	}
	return this->cache->get_texture(path);
}

const TextureManager &AssetManager::get_texture_manager() {
	return this->texture_manager;
}

} // namespace openage::renderer::resources
