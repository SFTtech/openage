// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "cache.h"

#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/palette_info.h"
#include "renderer/resources/terrain/blendpattern_info.h"
#include "renderer/resources/terrain/blendtable_info.h"
#include "renderer/resources/terrain/terrain_info.h"

namespace openage::renderer::resources {

const std::shared_ptr<Animation2dInfo> &AssetCache::get_animation(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_animations.contains(flat_path)) {
		return nullptr;
	}
	return this->loaded_animations[flat_path];
}


const std::shared_ptr<BlendPatternInfo> &AssetCache::get_blpattern(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_blpatterns.contains(flat_path)) {
		return nullptr;
	}
	return this->loaded_blpatterns[flat_path];
}


const std::shared_ptr<BlendTableInfo> &AssetCache::get_bltable(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_bltables.contains(flat_path)) {
		return nullptr;
	}
	return this->loaded_bltables[flat_path];
}


const std::shared_ptr<PaletteInfo> &AssetCache::get_palette(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_palettes.contains(flat_path)) {
		return nullptr;
	}
	return this->loaded_palettes[flat_path];
}


const std::shared_ptr<TerrainInfo> &AssetCache::get_terrain(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_terrains.contains(flat_path)) {
		return nullptr;
	}
	return this->loaded_terrains[flat_path];
}


const std::shared_ptr<Texture2dInfo> &AssetCache::get_texture(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded_textures.contains(flat_path)) {
		return nullptr;
	}
	return this->loaded_textures[flat_path];
}


void AssetCache::add_animation(const util::Path &path, const std::shared_ptr<Animation2dInfo> info) {
	auto flat_path = path.resolve_native_path();
	this->loaded_animations.insert({flat_path, info});
}


void AssetCache::add_blpattern(const util::Path &path, const std::shared_ptr<BlendPatternInfo> info) {
	auto flat_path = path.resolve_native_path();
	this->loaded_blpatterns.insert({flat_path, info});
}


void AssetCache::add_bltable(const util::Path &path, const std::shared_ptr<BlendTableInfo> info) {
	auto flat_path = path.resolve_native_path();
	this->loaded_bltables.insert({flat_path, info});
}


void AssetCache::add_palette(const util::Path &path, const std::shared_ptr<PaletteInfo> info) {
	auto flat_path = path.resolve_native_path();
	this->loaded_palettes.insert({flat_path, info});
}


void AssetCache::add_terrain(const util::Path &path, const std::shared_ptr<TerrainInfo> info) {
	auto flat_path = path.resolve_native_path();
	this->loaded_terrains.insert({flat_path, info});
}

void AssetCache::add_texture(const util::Path &path, const std::shared_ptr<Texture2dInfo> info) {
	auto flat_path = path.resolve_native_path();
	this->loaded_textures.insert({flat_path, info});
}

void AssetCache::remove_animation(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_animations.erase(flat_path);
}

void AssetCache::remove_blpattern(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_blpatterns.erase(flat_path);
}

void AssetCache::remove_bltable(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_bltables.erase(flat_path);
}

void AssetCache::remove_palette(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_palettes.erase(flat_path);
}

void AssetCache::remove_terrain(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_terrains.erase(flat_path);
}

void AssetCache::remove_texture(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded_textures.erase(flat_path);
}

bool AssetCache::check_animation_cache(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	return this->loaded_animations.contains(flat_path);
}

bool AssetCache::check_blpattern_cache(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	return this->loaded_blpatterns.contains(flat_path);
}

bool AssetCache::check_bltable_cache(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	return this->loaded_bltables.contains(flat_path);
}

bool AssetCache::check_palette_cache(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	return this->loaded_palettes.contains(flat_path);
}

bool AssetCache::check_terrain_cache(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	return this->loaded_terrains.contains(flat_path);
}

bool AssetCache::check_texture_cache(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	return this->loaded_textures.contains(flat_path);
}

} // namespace openage::renderer::resources
