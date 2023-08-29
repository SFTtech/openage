// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "texture_manager.h"

#include "renderer/renderer.h"
#include "renderer/resources/texture_data.h"


namespace openage::renderer::resources {

TextureManager::TextureManager(const std::shared_ptr<Renderer> &renderer) :
	renderer{renderer},
	loaded{} {
}

const std::shared_ptr<Texture2d> &TextureManager::request(const util::Path &path) {
	if (not this->loaded.contains(path)) {
		// create if not loaded
		auto tex_data = resources::Texture2dData(path);
		this->loaded.insert({path, this->renderer->add_texture(tex_data)});
	}
	return this->loaded.at(path);
}

void TextureManager::add(const util::Path &path) {
	if (not this->loaded.contains(path)) {
		// create if not loaded
		auto tex_data = resources::Texture2dData(path);
		this->loaded.insert({path, this->renderer->add_texture(tex_data)});
	}
}

void TextureManager::add(const util::Path &path,
                         const std::shared_ptr<Texture2d> &texture) {
	auto flat_path = path.resolve_native_path();
	this->loaded.insert({path, texture});
}

void TextureManager::remove(const util::Path &path) {
	this->loaded.erase(path);
}

void TextureManager::set_placeholder(const util::Path &path) {
	auto tex_data = resources::Texture2dData(path);
	this->placeholder = std::make_pair(path, this->renderer->add_texture(tex_data));
}

const TextureManager::placeholder_t &TextureManager::get_placeholder() const {
	return this->placeholder;
}

} // namespace openage::renderer::resources
