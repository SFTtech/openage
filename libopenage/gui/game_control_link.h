// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <QQmlParserStatus>

#include "../game_control.h"
#include "guisys/link/gui_item.h"

namespace openage {
namespace gui {

class GameControlLink;

class OutputModeLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::OutputMode> {
	using Type = openage::gui::OutputModeLink;
};

template<>
struct Unwrap<openage::gui::OutputModeLink> {
	using Type = openage::OutputMode;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class OutputModeLink : public qtsdl::GuiItemQObject, public QQmlParserStatus, public qtsdl::GuiItemInterface<OutputModeLink> {
	Q_OBJECT

	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QString name READ get_name NOTIFY name_changed)
	Q_PROPERTY(QStringList binds READ get_binds NOTIFY binds_changed)

public:
	OutputModeLink(QObject *parent=nullptr);
	virtual ~OutputModeLink();

	QString get_name() const;
	QStringList get_binds() const;

signals:
	void name_changed();
	void binds_changed();

private slots:
	void on_announced(const std::string &name);
	void on_binds_changed(const std::vector<std::string>& binds);

protected:
	virtual void classBegin() override;
	virtual void on_core_adopted() override;
	virtual void componentComplete() override;

private:
	QString name;
	QStringList binds;
};

class CreateModeLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::CreateMode> {
	using Type = openage::gui::CreateModeLink;
};

template<>
struct Unwrap<openage::gui::CreateModeLink> {
	using Type = openage::CreateMode;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class CreateModeLink : public qtsdl::Inherits<OutputModeLink, CreateModeLink> {
	Q_OBJECT

public:
	CreateModeLink(QObject *parent=nullptr);
	virtual ~CreateModeLink();
};

class ActionModeLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::ActionMode> {
	using Type = openage::gui::ActionModeLink;
};

template<>
struct Unwrap<openage::gui::ActionModeLink> {
	using Type = openage::ActionMode;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class ActionModeLink : public qtsdl::Inherits<OutputModeLink, ActionModeLink> {
	Q_OBJECT

	Q_PROPERTY(QString ability READ get_ability NOTIFY ability_changed)
	Q_PROPERTY(QString population READ get_population NOTIFY population_changed)
	Q_PROPERTY(bool population_warn READ get_population_warn NOTIFY population_changed)

public:
	ActionModeLink(QObject *parent=nullptr);
	virtual ~ActionModeLink();

	QString get_ability() const;
	QString get_population() const;
	bool get_population_warn() const;

	Q_INVOKABLE void act(const QString &action);

signals:
	void ability_changed();
	void action_triggered(const std::string &ability);
	void buttons_type_changed(const ActionButtonsType buttons_type);
	void population_changed();

private slots:
	void on_ability_changed(const std::string &ability);
	void on_buttons_type_changed(const ActionButtonsType buttons_type);
	void on_population_changed(int population, int population_cap, bool warn);

private:
	virtual void on_core_adopted() override;

	QString ability;
	QString population;
	bool population_warn;
};

class EditorModeLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::EditorMode> {
	using Type = openage::gui::EditorModeLink;
};

template<>
struct Unwrap<openage::gui::EditorModeLink> {
	using Type = openage::EditorMode;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class EditorModeLink : public qtsdl::Inherits<OutputModeLink, EditorModeLink> {
	Q_OBJECT

	Q_PROPERTY(int currentTypeId READ get_current_type_id WRITE set_current_type_id)
	Q_PROPERTY(int currentTerrainId READ get_current_terrain_id WRITE set_current_terrain_id)
	Q_PROPERTY(bool paintTerrain READ get_paint_terrain WRITE set_paint_terrain)
	Q_PROPERTY(QStringList categories READ get_categories NOTIFY categories_changed)

public:
	EditorModeLink(QObject *parent=nullptr);
	virtual ~EditorModeLink();

	int get_current_type_id() const;
	void set_current_type_id(int current_type_id);

	int get_current_terrain_id() const;
	void set_current_terrain_id(int current_terrain_id);

	bool get_paint_terrain() const;
	void set_paint_terrain(bool paint_terrain);

	QStringList get_categories() const;

	void announce_category_content(const std::string &category_name);

signals:
	void toggle();
	void categories_changed();
	void categories_content_changed();
	void category_content_changed(const std::string &category_name, std::vector<std::tuple<index_t, uint16_t>> type_and_texture);

private slots:
	void on_categories_changed(const std::vector<std::string> &categories);

private:
	virtual void on_core_adopted() override;

	int current_type_id;
	int current_terrain_id;
	bool paint_terrain;
	QStringList categories;
};

class EngineLink;
class GameMainLink;
class GameControlLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::GameControl> {
	using Type = openage::gui::GameControlLink;
};

template<>
struct Unwrap<openage::gui::GameControlLink> {
	using Type = openage::GameControl;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class GameControlLink : public qtsdl::GuiItemQObject, public QQmlParserStatus, public qtsdl::GuiItem<GameControlLink> {
	Q_OBJECT

	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(openage::gui::OutputModeLink* mode READ get_mode NOTIFY mode_changed)
	Q_PROPERTY(int effectiveModeIndex READ get_effective_mode_index NOTIFY mode_changed)
	Q_PROPERTY(int modeIndex READ get_mode_index WRITE set_mode_index)
	Q_PROPERTY(QVariantList modes READ get_modes WRITE set_modes NOTIFY modes_changed)
	Q_PROPERTY(openage::gui::EngineLink* engine READ get_engine WRITE set_engine)
	Q_PROPERTY(openage::gui::GameMainLink* game READ get_game WRITE set_game)
	Q_PROPERTY(QString currentPlayerName READ get_current_player_name NOTIFY current_player_name_changed)
	Q_PROPERTY(int currentCivIndex READ get_current_civ_index NOTIFY current_civ_index_changed)

public:
	explicit GameControlLink(QObject *parent=nullptr);
	virtual ~GameControlLink();

	OutputModeLink* get_mode() const;
	int get_effective_mode_index() const;

	int get_mode_index() const;
	void set_mode_index(int mode);

	QVariantList get_modes() const;
	void set_modes(const QVariantList &modes);

	EngineLink* get_engine() const;
	void set_engine(EngineLink *engine);

	GameMainLink* get_game() const;
	void set_game(GameMainLink *game);

	QString get_current_player_name() const;
	int get_current_civ_index() const;

signals:
	void mode_changed();
	void modes_changed();
	void current_player_name_changed();
	void current_civ_index_changed();

private slots:
	void on_mode_changed(OutputMode *mode, int mode_index);
	void on_modes_changed(OutputMode *mode, int mode_index);
	void on_current_player_name_changed(const std::string &current_player_name);
	void on_current_civ_index_changed(int current_civ_index);

private:
	virtual void classBegin() override;
	virtual void on_core_adopted() override;
	virtual void componentComplete() override;

	OutputModeLink *mode;
	int effective_mode_index;
	int mode_index;
	QVariantList modes;
	// TODO: remove engine because it's already accessible through the game
	EngineLink *engine;
	GameMainLink *game;
	QString current_player_name;
	int current_civ_index;
};

}} // namespace openage::gui
