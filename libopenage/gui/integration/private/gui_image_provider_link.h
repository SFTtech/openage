// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>
#include <QPointer>

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {

class GameSpec;

namespace gui {

class GuiGameSpecImageProviderImpl;
class GameSpecLink;

class GuiImageProviderLink : public QObject {
	Q_OBJECT

	Q_PROPERTY(openage::gui::GameSpecLink* gameSpec READ get_game_spec WRITE set_game_spec)

public:
	explicit GuiImageProviderLink(QObject *parent, GuiGameSpecImageProviderImpl &image_provider);
	virtual ~GuiImageProviderLink();

	GameSpecLink* get_game_spec() const;

	/**
	 * Sets the game spec to load textures from.
	 *
	 * Setting to null doesn't detach from the spec, but picks the
	 * 'missing texture' from that spec and sets it to every sprite.
	 */
	void set_game_spec(GameSpecLink *game_spec);

	static QObject* provider(QQmlEngine*, const char *id);
	static QObject* provider_by_filename(QQmlEngine*, QJSEngine*);
	static QObject* provider_by_graphic_id(QQmlEngine*, QJSEngine*);
	static QObject* provider_by_terrain_id(QQmlEngine*, QJSEngine*);

private slots:
	/**
	 * Pass loaded assets to the image provider.
	 *
	 * Need to check if we still attached to that spec.
	 * Also it may be invalidated in the meantime, so share the ownership.
	 */
	void on_game_spec_loaded(GameSpecLink *game_spec, std::shared_ptr<GameSpec> loaded_game_spec);

private:
	GuiGameSpecImageProviderImpl &image_provider;
	QPointer<GameSpecLink> game_spec;
};

}} // namespace openage::gui
