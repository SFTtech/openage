// Copyright 2013-2018 the openage authors. See copying.md for legal info.

#include "console.h"

#include "../log/log.h"
#include "../error/error.h"
#include "../util/strings.h"
#include "../util/unicode.h"
#include "../engine.h"

#include "draw.h"

namespace openage {
namespace console {

/*
 * TODO:
 * multiple terminals on screen
 * resizable terminals
 * scrollbars
 * console input
 * log console, command console
 */

Console::Console(Engine *engine)
	:
	engine{engine},
	bottomleft{0, 0},
	topright{1, 1},
	charsize{1, 1},
	visible(false),
	buf{{80, 25}, 1337, 80},
	font{{"DejaVu Sans Mono", "Book", 12}} {

	termcolors.reserve(256);

	// this better be representative for the width of all other characters
	charsize.x = ceilf(font.get_advance_width("W"));
	charsize.y = ceilf(font.get_line_height());

	log::log(MSG(dbg) << "Console font character size: " << charsize.x << "x" << charsize.y);

	// Adjust the corners of the console based on the default buffer size and char size
	topright.x = charsize.x * this->buf.get_dims().x;
	topright.y = charsize.y * this->buf.get_dims().y;
}

Console::~Console () {}

void Console::load_colors(std::vector<gamedata::palette_color> &colortable) {
	for (auto &c : colortable) {
		this->termcolors.emplace_back(c);
	}

	if (termcolors.size() != 256) {
		throw Error(MSG(err) << "Exactly 256 terminal colors are required.");
	}
}

void Console::register_to_engine() {
	this->engine->register_input_action(this);
	this->engine->register_tick_action(this);
	this->engine->register_drawhud_action(this);
	this->engine->register_resize_action(this);

	// Bind the console toggle key globally
	auto &action = this->engine->get_action_manager();
	auto &global = this->engine->get_input_manager().get_global_context();

	global.bind(action.get("TOGGLE_CONSOLE"), [this](const input::action_arg_t &) {
		this->set_visible(!this->visible);
	});


	// TODO: bind any needed input to InputContext

	// toggle console will take highest priority
	this->input_context.bind(action.get("TOGGLE_CONSOLE"), [this](const input::action_arg_t &) {
		this->set_visible(false);
	});
	this->input_context.bind(input::event_class::UTF8, [this](const input::action_arg_t &arg) {
		// a single char typed into the console
		std::string utf8 = arg.e.as_utf8();
		this->buf.write(utf8.c_str());
		command += utf8;
		return true;
	});
	this->input_context.bind(input::event_class::NONPRINT, [this](const input::action_arg_t &arg) {
		switch (arg.e.as_char()) {
			case 8: // remove a single UTF-8 character
				if (this->command.size() > 0) {
					util::utf8_pop_back(this->command);
					this->buf.pop_last_char();
				}
				return true;

			case 13: // interpret command
				this->buf.write('\n');
				this->interpret(this->command);
				this->command = "";
				return true;

			default:
				return false;
		}
	});
	this->input_context.utf8_mode = true;
}

void Console::set_visible(bool make_visible) {
	if (make_visible) {
		this->engine->get_input_manager().push_context(&this->input_context);
		this->visible = true;
	}
	else {
		this->engine->get_input_manager().remove_context(&this->input_context);
		this->visible = false;
	}
}

void Console::write(const char *text) {
	this->buf.write(text);
	this->buf.write('\n');
}

void Console::interpret(const std::string &command) {
	if (command == "exit") {
		this->set_visible(false);
	}
	else if (command == "list") {
		for (auto &line : this->engine->list_options()) {
			this->write(line.c_str());
		}
	}
	else if (command.substr(0,3) ==  "set") {
		std::size_t first_space = command.find(" ");
		std::size_t second_space = command.find(" ", first_space+1);
		if (first_space != std::string::npos && second_space != std::string::npos) {
			std::string name = command.substr(first_space+1, second_space-first_space-1);
			std::string value = command.substr(second_space+1, std::string::npos);
			this->engine->get_cvar_manager().set(name,value);
		}
	}
	else if (command.substr(0,3) == "get") {
		std::size_t first_space = command.find(" ");
		if (first_space != std::string::npos) {
			std::string name = command.substr(first_space+1, std::string::npos);
			std::string value = this->engine->get_cvar_manager().get(name);
			this->write(value.c_str());
		}
	}
}

bool Console::on_tick() {
	if (!this->visible) {
		return true;
	}

	// TODO: handle stuff such as cursor blinking,
	// repeating held-down keys
	return true;
}

bool Console::on_drawhud() {
	if (!this->visible) {
		return true;
	}

	draw::to_opengl(this->engine, this);

	return true;
}

bool Console::on_input(SDL_Event *e) {
	// only handle inputs if the console is visible
	if (!this->visible) {
		return true;
	}

	switch (e->type) {
	case SDL_KEYDOWN:
		//TODO handle key inputs

		//do not allow anyone else to handle this input
		return false;
		break;
	}

	return true;
}

bool Console::on_resize(coord::viewport_delta new_size) {
	coord::pixel_t w = this->buf.get_dims().x * this->charsize.x;
	coord::pixel_t h = this->buf.get_dims().y * this->charsize.y;

	this->bottomleft = {(new_size.x - w) / 2, (new_size.y - h) / 2};
	this->topright = {this->bottomleft.x + w, this->bottomleft.y - h};

	return true;
}

}} // openage::console
