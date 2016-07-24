// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <memory>
#include <chrono>

#include <QObject>

#include "action.h"
#include "action_serialization.h"

namespace qtsdl {
class GuiItemLink;
} // qtsdl

namespace openage {
namespace input {

class InputContext;
class ActionManager;

class InputContextRecorderPlayer {
public:
	explicit InputContextRecorderPlayer(qtsdl::GuiItemLink *gui_link, bool recorder);
#ifndef NDEBUG
	virtual ~InputContextRecorderPlayer() {}
#endif

	std::string get_file_name() const;
	void set_file_name(const std::string &file_name);

	virtual void announce();

protected:
	std::string file_name;
	std::unique_ptr<std::iostream> stream;
	const bool recorder;

public:
	qtsdl::GuiItemLink *gui_link;
};

class InputContextRecorder : public InputContextRecorderPlayer {
public:
	explicit InputContextRecorder(qtsdl::GuiItemLink *gui_link);

	void save_action(const action_arg_t &arg, const ActionManager &action_manager);

private:
	/**
	 * Moment when the recording has started.
	 */
	std::chrono::microseconds start_time;
	bool started;
};

class InputContextPlayerSignals : public QObject {
	Q_OBJECT

public:
signals:
	void mouse_changed(const coord::window &mouse);
};

class InputContextPlayer : public InputContextRecorderPlayer {
public:
	explicit InputContextPlayer(qtsdl::GuiItemLink *gui_link);

	void perform(InputContext &context, const ActionManager &action_manager);

	/**
	 * after the gui reload sends to it the properties that are needed there
	 */
	virtual void announce() override;

private:
	void store_mouse_pos(const action_arg_t &arg);

	/**
	 * Moment when the playback has started.
	 */
	std::chrono::microseconds start_time;
	bool started;

	std::chrono::microseconds next_action_time;
	deserialized_action_arg_t next_action;

	bool ended;

	coord::window mouse;

public:
	InputContextPlayerSignals gui_signals;
};

}} // openage::input
