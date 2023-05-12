// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "texture_manager.h"

#include "renderer/renderer.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/texture_data.h"

namespace openage::renderer::resources {

TextureManager::TextureManager(const std::shared_ptr<Renderer> &renderer) :
	renderer{renderer},
	loaded{} {
}

const std::shared_ptr<Texture2d> &TextureManager::request(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded.contains(flat_path)) {
		// create if not loaded
		auto tex_data = resources::Texture2dData(path);
		this->loaded.insert({flat_path, this->renderer->add_texture(tex_data)});
	}
	return this->loaded.at(flat_path);
}

void TextureManager::add(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	if (not this->loaded.contains(flat_path)) {
		// create if not loaded
		auto tex_data = resources::Texture2dData(path);
		this->loaded.insert({flat_path, this->renderer->add_texture(tex_data)});
	}
}

void TextureManager::add(const util::Path &path,
                         const std::shared_ptr<Texture2d> &texture) {
	auto flat_path = path.resolve_native_path();
	this->loaded.insert({flat_path, texture});
}

void TextureManager::remove(const util::Path &path) {
	auto flat_path = path.resolve_native_path();
	this->loaded.erase(flat_path);
}

} // namespace openage::renderer::resources
