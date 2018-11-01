// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <QQmlParserStatus>

#include "guisys/link/gui_item.h"

#include "../gamestate/old/game_main.h"

namespace openage {
namespace gui {

class EngineLink;
class GameMainLink;

}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::GameMainHandle> {
	using Type = openage::gui::GameMainLink;
};

template<>
struct Unwrap<openage::gui::GameMainLink> {
	using Type = openage::GameMainHandle;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class GameMainLink : public qtsdl::GuiItemQObject, public QQmlParserStatus, public qtsdl::GuiItem<GameMainLink> {
	Q_OBJECT

	Q_INTERFACES(QQmlParserStatus)
	Q_ENUMS(State)
	Q_PROPERTY(State state READ get_state NOTIFY state_changed)
	Q_PROPERTY(openage::gui::EngineLink* engine READ get_engine WRITE set_engine)

public:
	explicit GameMainLink(QObject *parent=nullptr);
	virtual ~GameMainLink();

	enum class State {
		Null, Running
	};

	State get_state() const;

	EngineLink* get_engine() const;
	void set_engine(EngineLink *engine);

	Q_INVOKABLE	void clear();

signals:
	void state_changed();

private slots:
	void on_game_running(bool running);

private:
	virtual void classBegin() override;
	virtual void on_core_adopted() override;
	virtual void componentComplete() override;

	State state;
	bool active;
	EngineLink *engine;
};

}} // namespace openage::gui
