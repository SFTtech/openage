// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <tuple>
#include <condition_variable>

#include "../../guisys/private/gui_image_provider_impl.h"
#include "../../guisys/private/gui_callback.h"
#include "gui_texture_handle.h"
#include "gui_filled_texture_handles.h"

namespace qtsdl {

class GuiEventQueue;
class GuiEventQueueImpl;

} // namespace qtsdl

namespace openage {

class GameSpec;
class Texture;

namespace gui {

class GuiTexture;

/**
 * Exposes game textures to the Qt.
 */
class GuiGameSpecImageProviderImpl : public qtsdl::GuiImageProviderImpl {
public:
	explicit GuiGameSpecImageProviderImpl(qtsdl::GuiEventQueue *render_updater);
	virtual ~GuiGameSpecImageProviderImpl();

	/**
	 * Unblocks the provider.
	 *
	 * Refreshes already loaded assets (if this->loaded_game_spec wasn't null before the call).
	 *
	 * @param loaded_game_spec new source (can't be null)
	 */
	void on_game_spec_loaded(const std::shared_ptr<GameSpec>& loaded_game_spec);

	/**
	 * Set to every sprite the 'missing texture' from current spec.
	 *
	 * Needed as a visible reaction to setting the property to null.
	 * We can't unload the textures without recreating the engine, so we keep the old source.
	 */
	void on_game_spec_invalidated();

	/**
	 * Fills in the provided handle object.
	 *
	 * Memorizes pointer to it in order to update it if the source changes.
	 *
	 * @return pointer to all memorized handles, so the client can unsubscribe.
	 */
	GuiFilledTextureHandleUser fill_texture_handle(const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle);

protected:
	TextureHandle get_missing_texture();

	std::shared_ptr<GameSpec> loaded_game_spec;

	/**
	 * When true, still uses the old source but shows the 'missing texture'.
	 */
	bool invalidated;

private:
	virtual TextureHandle get_texture_handle(const QString &id) = 0;
	virtual QQuickTextureFactory* requestTexture(const QString &id, QSize *size, const QSize &requestedSize) override;
	virtual void give_up() override;

	/**
	 * Change the already produced texture handles to use new source.
	 */
	void migrate_to_new_game_spec(const std::shared_ptr<GameSpec>& loaded_game_spec);

	void overwrite_texture_handle(const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle);

	/**
	 * Changing the texture handles underneath the QSGTexture to reflect the reload of the GameSpec.
	 *
	 * It's not a proper Qt usage, so the live reload of the game assets for the gui may break in future Qt releases.
	 * When it breaks, this feature should be implemented via the recreation of the qml engine.
	 */
	std::shared_ptr<GuiFilledTextureHandles> filled_handles;

	std::mutex loaded_game_spec_mutex;
	std::condition_variable loaded_game_spec_cond;
	bool ended;

	qtsdl::GuiCallback render_thread_callback;
};

}} // namespace openage::gui
