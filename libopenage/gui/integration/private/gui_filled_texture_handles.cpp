// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "gui_filled_texture_handles.h"

#include "gui_texture_handle.h"

namespace openage {
namespace gui {

GuiFilledTextureHandles::GuiFilledTextureHandles() {
}

GuiFilledTextureHandles::~GuiFilledTextureHandles() {
}

void GuiFilledTextureHandles::add_texture_handle(const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle) {
	std::unique_lock<std::mutex> lck{this->handles_mutex};
	this->handles.push_back(std::make_tuple(id, requested_size, filled_handle));
}

void GuiFilledTextureHandles::free_texture_handle(SizedTextureHandle *filled_handle) {
	std::unique_lock<std::mutex> lck{this->handles_mutex};
	this->handles.erase(std::remove_if(std::begin(this->handles), std::end(this->handles), [filled_handle] (const std::tuple<QString, QSize, SizedTextureHandle*>& handle) {
		return std::get<SizedTextureHandle*>(handle) == filled_handle;
	}), std::end(this->handles));
}

void GuiFilledTextureHandles::fill_all_handles_with_texture(const TextureHandle &texture) {
	std::unique_lock<std::mutex> lck{this->handles_mutex};
	std::for_each(std::begin(this->handles), std::end(this->handles), [&texture] (std::tuple<QString, QSize, SizedTextureHandle*>& handle) {
		auto filled_handle = std::get<SizedTextureHandle*>(handle);
		*filled_handle = {texture, textureSize(*filled_handle)};
	});
}

void GuiFilledTextureHandles::refresh_all_handles_with_texture(std::function<void(const QString&, const QSize&, SizedTextureHandle*)> refresher) {
	std::unique_lock<std::mutex> lck{this->handles_mutex};

	std::vector<SizedTextureHandle> refreshed_handles(this->handles.size());

	std::transform(std::begin(this->handles), std::end(this->handles), std::begin(refreshed_handles), [refresher] (const std::tuple<QString, QSize, SizedTextureHandle*>& handle) {
		SizedTextureHandle refreshed_handles;
		refresher(std::get<QString>(handle), std::get<QSize>(handle), &refreshed_handles);
		return refreshed_handles;
	});

	for (std::size_t i = 0, e = refreshed_handles.size(); i != e; ++i)
		*std::get<SizedTextureHandle*>(this->handles[i]) = refreshed_handles[i];
}

GuiFilledTextureHandleUser::GuiFilledTextureHandleUser(const std::shared_ptr<GuiFilledTextureHandles> &texture_handles, const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle)
	:
	texture_handles{texture_handles},
	filled_handle{filled_handle} {

	this->texture_handles->add_texture_handle(id, requested_size, filled_handle);
}

GuiFilledTextureHandleUser::~GuiFilledTextureHandleUser() {
	// Assume that the factory lives longer than the textures it has created.
	texture_handles->free_texture_handle(filled_handle);
}

}} // namespace openage::gui
