// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "asset_manager.h"

#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/palette_info.h"
#include "renderer/resources/parser/parse_blendmask.h"
#include "renderer/resources/parser/parse_blendtable.h"
#include "renderer/resources/parser/parse_palette.h"
#include "renderer/resources/parser/parse_sprite.h"
#include "renderer/resources/parser/parse_terrain.h"
#include "renderer/resources/terrain/blendpattern_info.h"
#include "renderer/resources/terrain/blendtable_info.h"
#include "renderer/resources/terrain/terrain_info.h"

namespace openage::renderer::resources {

AssetManager::AssetManager(const std::shared_ptr<Renderer> &renderer) :
	texture_manager{renderer},
	loaded_animations{},
	loaded_blpatterns{},
	loaded_bltables{},
	loaded_palettes{},
	loaded_terrains{} {
}

const std::shared_ptr<Animation2dInfo> &AssetManager::request_animation(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_animations.contains(flat_path)) {
		// create if not loaded
		auto anim_info = parser::parse_sprite_file(path, this->texture_manager.get_cache());
		auto anim = std::make_shared<Animation2dInfo>(std::move(anim_info));
		this->loaded_animations.insert({flat_path, anim});
	}
	return this->loaded_animations[flat_path];
}

const std::shared_ptr<BlendPatternInfo> &AssetManager::request_blpattern(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_blpatterns.contains(flat_path)) {
		// create if not loaded
		auto pattern_info = parser::parse_blendmask_file(path, this->texture_manager.get_cache());
		auto pattern = std::make_shared<BlendPatternInfo>(std::move(pattern_info));
		this->loaded_blpatterns.insert({flat_path, pattern});
	}
	return this->loaded_blpatterns[flat_path];
}

const std::shared_ptr<BlendTableInfo> &AssetManager::request_bltable(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_bltables.contains(flat_path)) {
		// create if not loaded
		auto table_info = parser::parse_blendtable_file(path, this->loaded_blpatterns);
		auto table = std::make_shared<BlendTableInfo>(std::move(table_info));
		this->loaded_bltables.insert({flat_path, table});
	}
	return this->loaded_bltables[flat_path];
}

const std::shared_ptr<PaletteInfo> &AssetManager::request_palette(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_palettes.contains(flat_path)) {
		// create if not loaded
		auto palette_info = parser::parse_palette_file(path);
		auto palette = std::make_shared<PaletteInfo>(std::move(palette_info));
		this->loaded_palettes.insert({flat_path, palette});
	}
	return this->loaded_palettes[flat_path];
}

const std::shared_ptr<TerrainInfo> &AssetManager::request_terrain(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_terrains.contains(flat_path)) {
		// create if not loaded
		auto terrain_info = parser::parse_terrain_file(path,
		                                               this->texture_manager.get_cache(),
		                                               this->loaded_bltables);
		auto terrain = std::make_shared<TerrainInfo>(std::move(terrain_info));
		this->loaded_terrains.insert({flat_path, terrain});
	}
	return this->loaded_terrains[flat_path];
}

void AssetManager::remove_animation(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_animations.erase(flat_path);
}
void AssetManager::remove_blpattern(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_blpatterns.erase(flat_path);
}
void AssetManager::remove_bltable(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_bltables.erase(flat_path);
}
void AssetManager::remove_palette(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_palettes.erase(flat_path);
}
void AssetManager::remove_terrain(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_terrains.erase(flat_path);
}

} // namespace openage::renderer::resources
