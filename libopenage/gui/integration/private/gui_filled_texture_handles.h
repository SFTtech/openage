// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <mutex>
#include <memory>
#include <functional>

#include <QString>
#include <QSize>

namespace openage {
namespace gui {

class TextureHandle;
class SizedTextureHandle;

/**
 * Stores pointers to game texture handlers given to the Qt.
 */
class GuiFilledTextureHandles {
public:
	explicit GuiFilledTextureHandles();
	~GuiFilledTextureHandles();

	/**
	 * Memorizes pointer to provided handle object in order to be able to update it if the source changes.
	 */
	void add_texture_handle(const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle);
	void free_texture_handle(SizedTextureHandle *filled_handle);

	void fill_all_handles_with_texture(const TextureHandle &texture);
	void refresh_all_handles_with_texture(std::function<void(const QString&, const QSize&, SizedTextureHandle*)> refresher);

private:
	/**
	 * Changing the texture handles underneath the QSGTexture to reflect the reload of the GameSpec.
	 *
	 * It's not a proper Qt usage, so the live reload of the game assets for the gui may break in future Qt releases.
	 * When it breaks, this feature should be implemented via the recreation of the qml engine.
	 */
	std::vector<std::tuple<QString, QSize, SizedTextureHandle*>> handles;
	std::mutex handles_mutex;
};

class GuiFilledTextureHandleUser {
public:
	GuiFilledTextureHandleUser(const std::shared_ptr<GuiFilledTextureHandles> &texture_handles, const QString &id, const QSize &requested_size, SizedTextureHandle *filled_handle);
	~GuiFilledTextureHandleUser();

	GuiFilledTextureHandleUser(GuiFilledTextureHandleUser&&) noexcept = default;

private:
	GuiFilledTextureHandleUser(const GuiFilledTextureHandleUser&) = delete;
	GuiFilledTextureHandleUser& operator=(const GuiFilledTextureHandleUser&) = delete;

	std::shared_ptr<GuiFilledTextureHandles> texture_handles;
	SizedTextureHandle *filled_handle;
};

}} // namespace openage::gui
