// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider_impl.h"

#include <algorithm>

#include <QThread>

#include "../../../error/error.h"

#include "../../../gamestate/game_spec.h"
#include "../../guisys/private/gui_event_queue_impl.h"

#include "gui_texture_factory.h"
#include "gui_filled_texture_handles.h"

namespace openage {
namespace gui {

GuiGameSpecImageProviderImpl::GuiGameSpecImageProviderImpl(qtsdl::GuiEventQueue *render_updater)
	:
	GuiImageProviderImpl{},
	invalidated{},
	filled_handles{std::make_shared<GuiFilledTextureHandles>()},
	ended{} {

	QThread *render_thread = qtsdl::GuiEventQueueImpl::impl(render_updater)->get_thread();
	this->render_thread_callback.moveToThread(render_thread);
	QObject::connect(&this->render_thread_callback, &qtsdl::GuiCallback::process_blocking, &this->render_thread_callback, &qtsdl::GuiCallback::process, render_thread != QThread::currentThread() ? Qt::BlockingQueuedConnection : Qt::DirectConnection);
}

GuiGameSpecImageProviderImpl::~GuiGameSpecImageProviderImpl() {
}

void GuiGameSpecImageProviderImpl::on_game_spec_loaded(const std::shared_ptr<GameSpec>& loaded_game_spec) {
	ENSURE(loaded_game_spec, "spec hasn't been checked or was invalidated");

	std::unique_lock<std::mutex> lck{this->loaded_game_spec_mutex};

	if (invalidated || loaded_game_spec != this->loaded_game_spec) {
		migrate_to_new_game_spec(loaded_game_spec);
		invalidated = false;

		lck.unlock();
		this->loaded_game_spec_cond.notify_one();
	}
}

void GuiGameSpecImageProviderImpl::migrate_to_new_game_spec(const std::shared_ptr<GameSpec>& loaded_game_spec) {
	ENSURE(loaded_game_spec, "spec hasn't been checked or was invalidated");

	if (this->loaded_game_spec) {
		auto keep_old_game_spec = this->loaded_game_spec;

		// Need to set up this because the load functions use this->loaded_game_spec internally.
		this->loaded_game_spec = loaded_game_spec;

		emit this->render_thread_callback.process_blocking([this] {
			using namespace std::placeholders;
			this->filled_handles->refresh_all_handles_with_texture(std::bind(&GuiGameSpecImageProviderImpl::overwrite_texture_handle, this, _1, _2, _3));
		});
	} else {
		this->loaded_game_spec = loaded_game_spec;
	}
}

void GuiGameSpecImageProviderImpl::overwrite_texture_handle(const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle) {
	const TextureHandle texture_handle = this->get_texture_handle(id);
	*filled_handle = {texture_handle, aspect_fit_size(texture_handle, requested_size)};
}

void GuiGameSpecImageProviderImpl::on_game_spec_invalidated() {
	std::unique_lock<std::mutex> lck{this->loaded_game_spec_mutex};

	if (this->loaded_game_spec) {
		const TextureHandle missing_texture = get_missing_texture();

		emit this->render_thread_callback.process_blocking([this, &missing_texture] {
			this->filled_handles->fill_all_handles_with_texture(missing_texture);
		});

		invalidated = true;
	}
}

GuiFilledTextureHandleUser GuiGameSpecImageProviderImpl::fill_texture_handle(const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle) {
	this->overwrite_texture_handle(id, requested_size, filled_handle);
	return GuiFilledTextureHandleUser(this->filled_handles, id, requested_size, filled_handle);
}

TextureHandle GuiGameSpecImageProviderImpl::get_missing_texture() {
	return TextureHandle{this->loaded_game_spec->get_texture("missing.png", false), -1};
}

QQuickTextureFactory* GuiGameSpecImageProviderImpl::requestTexture(const QString &id, QSize *size, const QSize &requestedSize) {
	std::unique_lock<std::mutex> lck{this->loaded_game_spec_mutex};

	this->loaded_game_spec_cond.wait(lck, [this] {return this->ended || this->loaded_game_spec;});

	if (this->ended) {
		qWarning("ImageProvider was stopped during the load, so it'll appear like the requestTexture() isn't implemented.");
		return this->GuiImageProviderImpl::requestTexture(id, size, requestedSize);
	} else {
		auto tex_factory = new GuiTextureFactory{this, id, requestedSize};
		*size = tex_factory->textureSize();
		return tex_factory;
	}
}

void GuiGameSpecImageProviderImpl::give_up() {
	{
		std::unique_lock<std::mutex> lck{this->loaded_game_spec_mutex};
		this->ended = true;
	}

	this->loaded_game_spec_cond.notify_one();
}

}} // namespace openage::gui
