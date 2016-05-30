// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QMetaType>
#include <QQmlParserStatus>

#include "guisys/link/gui_item.h"

#include "../gamestate/game_spec.h"

namespace openage {

class GameSpec;

namespace gui {

class AssetManagerLink;
class GameSpecLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::GameSpecHandle> {
	using Type = openage::gui::GameSpecLink;
};

template<>
struct Unwrap<openage::gui::GameSpecLink> {
	using Type = openage::GameSpecHandle;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class GameSpecLink : public qtsdl::GuiItemQObject, public QQmlParserStatus, public qtsdl::GuiItem<GameSpecLink> {
	Q_OBJECT

	Q_INTERFACES(QQmlParserStatus)
	Q_ENUMS(State)
	Q_PROPERTY(State state READ get_state NOTIFY state_changed)
	Q_PROPERTY(bool active READ get_active WRITE set_active)
	Q_PROPERTY(openage::gui::AssetManagerLink* assetManager READ get_asset_manager WRITE set_asset_manager)
	Q_PROPERTY(int terrainIdCount READ get_terrain_id_count NOTIFY terrain_id_count_changed)

public:
	explicit GameSpecLink(QObject *parent=nullptr);
	virtual ~GameSpecLink();

	enum class State {
		Null, Loading, Ready
	};

	State get_state() const;

	bool get_active() const;
	void set_active(bool active);

	AssetManagerLink* get_asset_manager() const;
	void set_asset_manager(AssetManagerLink *asset_manager);

	int get_terrain_id_count() const;

	Q_INVOKABLE void invalidate();

	std::shared_ptr<GameSpec> get_loaded_spec();

signals:
	/**
	 * Pass loaded assets to the image provider.
	 *
	 * Provider will check if it's still attached to that spec.
	 * Also it may be invalidated in the meantime, so share the ownership.
	 */
	void game_spec_loaded(GameSpecLink *game_spec, std::shared_ptr<GameSpec> loaded_game_spec);

	void state_changed();
	void terrain_id_count_changed();

private slots:
	void on_load_job_finished();
	void on_game_spec_loaded(std::shared_ptr<GameSpec> loaded_game_spec);

private:
	virtual void classBegin() override;
	virtual void on_core_adopted() override;
	virtual void componentComplete() override;

	void set_state(State state);

	State state;
	bool active;
	AssetManagerLink *asset_manager;
	int terrain_id_count;

	std::shared_ptr<GameSpec> loaded_game_spec;
};

}} // namespace openage::gui

Q_DECLARE_METATYPE(std::shared_ptr<openage::GameSpec>)
