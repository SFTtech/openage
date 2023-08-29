// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "cache.h"

#include "util/path.h"


namespace openage::renderer::resources {

const std::shared_ptr<Animation2dInfo> &AssetCache::get_animation(const util::Path &path) {
	return this->loaded_animations.at(path);
}


const std::shared_ptr<BlendPatternInfo> &AssetCache::get_blpattern(const util::Path &path) {
	return this->loaded_blpatterns.at(path);
}


const std::shared_ptr<BlendTableInfo> &AssetCache::get_bltable(const util::Path &path) {
	return this->loaded_bltables.at(path);
}


const std::shared_ptr<PaletteInfo> &AssetCache::get_palette(const util::Path &path) {
	return this->loaded_palettes.at(path);
}


const std::shared_ptr<TerrainInfo> &AssetCache::get_terrain(const util::Path &path) {
	return this->loaded_terrains.at(path);
}


const std::shared_ptr<Texture2dInfo> &AssetCache::get_texture(const util::Path &path) {
	return this->loaded_textures.at(path);
}


void AssetCache::add_animation(const util::Path &path, const std::shared_ptr<Animation2dInfo> info) {
	this->loaded_animations.insert({path, info});
}


void AssetCache::add_blpattern(const util::Path &path, const std::shared_ptr<BlendPatternInfo> info) {
	this->loaded_blpatterns.insert({path, info});
}


void AssetCache::add_bltable(const util::Path &path, const std::shared_ptr<BlendTableInfo> info) {
	this->loaded_bltables.insert({path, info});
}


void AssetCache::add_palette(const util::Path &path, const std::shared_ptr<PaletteInfo> info) {
	this->loaded_palettes.insert({path, info});
}


void AssetCache::add_terrain(const util::Path &path, const std::shared_ptr<TerrainInfo> info) {
	this->loaded_terrains.insert({path, info});
}

void AssetCache::add_texture(const util::Path &path, const std::shared_ptr<Texture2dInfo> info) {
	this->loaded_textures.insert({path, info});
}

void AssetCache::remove_animation(const util::Path &path) {
	this->loaded_animations.erase(path);
}

void AssetCache::remove_blpattern(const util::Path &path) {
	this->loaded_blpatterns.erase(path);
}

void AssetCache::remove_bltable(const util::Path &path) {
	this->loaded_bltables.erase(path);
}

void AssetCache::remove_palette(const util::Path &path) {
	this->loaded_palettes.erase(path);
}

void AssetCache::remove_terrain(const util::Path &path) {
	this->loaded_terrains.erase(path);
}

void AssetCache::remove_texture(const util::Path &path) {
	this->loaded_textures.erase(path);
}

bool AssetCache::check_animation_cache(const util::Path &path) {
	return this->loaded_animations.contains(path);
}

bool AssetCache::check_blpattern_cache(const util::Path &path) {
	return this->loaded_blpatterns.contains(path);
}

bool AssetCache::check_bltable_cache(const util::Path &path) {
	return this->loaded_bltables.contains(path);
}

bool AssetCache::check_palette_cache(const util::Path &path) {
	return this->loaded_palettes.contains(path);
}

bool AssetCache::check_terrain_cache(const util::Path &path) {
	return this->loaded_terrains.contains(path);
}

bool AssetCache::check_texture_cache(const util::Path &path) {
	return this->loaded_textures.contains(path);
}

} // namespace openage::renderer::resources
