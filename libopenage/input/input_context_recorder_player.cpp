// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "input_context_recorder_player.h"

#include <fstream>

#include "../error/error.h"

#include "input_context.h"

namespace openage {
namespace input {

namespace {
std::chrono::microseconds current_moment() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch());
}
}

InputContextRecorderPlayer::InputContextRecorderPlayer(qtsdl::GuiItemLink *gui_link, bool recorder)
	:
	stream{std::make_unique<std::iostream>(recorder ? std::cerr.rdbuf() : std::cin.rdbuf())},
	recorder{recorder},
	gui_link{gui_link} {
}

std::string InputContextRecorderPlayer::get_file_name() const {
	return this->file_name;
}

void InputContextRecorderPlayer::set_file_name(const std::string &file_name) {
	if (this->file_name != file_name) {
		this->stream = std::make_unique<std::fstream>(file_name, this->recorder ? std::ios_base::out : std::ios_base::in);
		this->file_name = file_name;
	}
}

void InputContextRecorderPlayer::announce() {
}

InputContextRecorder::InputContextRecorder(qtsdl::GuiItemLink *gui_link)
	:
	InputContextRecorderPlayer{gui_link, true},
	started{} {
}

void InputContextRecorder::save_action(const action_arg_t &arg, const ActionManager &action_manager) {
	if (!this->started) {
		this->start_time = current_moment();
		this->started = true;
	}

	*this->stream << (current_moment() - this->start_time).count() << ' ' << std::forward_as_tuple(arg, action_manager) << '\n';
}

InputContextPlayer::InputContextPlayer(qtsdl::GuiItemLink *gui_link)
	:
	InputContextRecorderPlayer{gui_link, false},
	started{},
	next_action{DeserializedEvent{DeserializedClassCode{event_class::ANY, 0}, modset_t{}, ""}, coord::window{}, coord::window_delta{}, std::vector<action_id_t>{}},
	ended{} {
}

namespace {
std::tuple<std::chrono::microseconds, deserialized_action_arg_t, bool> read_action(std::istream &i, const ActionManager &action_manager) {
	std::chrono::microseconds::rep next_action_time;
	deserialized_action_arg_t next_action{DeserializedEvent{DeserializedClassCode{event_class::ANY, 0}, modset_t{}, ""}, coord::window{}, coord::window_delta{}, std::vector<action_id_t>{}};
	const bool ended = !(i >> next_action_time >> std::forward_as_tuple(next_action, action_manager)).good();

	return std::make_tuple(std::chrono::microseconds{next_action_time}, next_action, ended);
}
}

void InputContextPlayer::perform(InputContext &context, const ActionManager &action_manager) {
	ENSURE(this->started || (!this->started && !this->ended), "Set ended to false when restarting.");

	if (!this->started) {
		this->start_time = current_moment();
		this->started = true;

		std::tie(this->next_action_time, this->next_action, this->ended) = read_action(*this->stream, action_manager);
		std::cerr << "got0 " << std::forward_as_tuple(to_action(this->next_action), action_manager) << "\n";
	}

	if (!this->ended) {
		const auto current = current_moment() - this->start_time;

		while (!this->ended) {
			if (this->next_action_time < current) {
				std::cerr << "performing " << std::forward_as_tuple(to_action(this->next_action), action_manager) << "\n";
				auto action = to_action(this->next_action);
				this->store_mouse_pos(action);
				context.execute_if_bound(action);
			} else {
				break;
			}

			std::tie(this->next_action_time, this->next_action, this->ended) = read_action(*this->stream, action_manager);
			std::cerr << "got1 " << std::forward_as_tuple(to_action(this->next_action), action_manager) << "\n";
		}
	}
}

void InputContextPlayer::announce() {
	this->InputContextRecorderPlayer::announce();
	emit this->gui_signals.mouse_changed(this->mouse);
}

void InputContextPlayer::store_mouse_pos(const action_arg_t &arg) {
	if (arg.e.cc.eclass == input::event_class::MOUSE_MOTION && !(this->mouse == arg.mouse)) {
		this->mouse = arg.mouse;
		emit this->gui_signals.mouse_changed(this->mouse);
	}
}

}} // openage::input
