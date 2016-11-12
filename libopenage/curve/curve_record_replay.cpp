// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "curve_record_replay.h"

#include <iostream>
#include <fstream>

#include "../unit/attribute_setter.h"

namespace openage {
namespace curve {

CurveRecordReplay::CurveRecordReplay(qtsdl::GuiItemLink *gui_link, bool recorder)
	:
	stream{std::make_unique<std::iostream>(recorder ? std::cerr.rdbuf() : std::cin.rdbuf())},
	current_frame{},
	recorder{recorder},
	gui_link{gui_link} {
}

CurveRecordReplay::~CurveRecordReplay() {

}

std::string CurveRecordReplay::get_file_name() const {
	return this->file_name;
}

void CurveRecordReplay::set_file_name(const std::string &file_name) {
	if (this->file_name != file_name) {
		this->stream = std::make_unique<std::fstream>(file_name, this->recorder ? std::ios_base::out : std::ios_base::in);
		this->file_name = file_name;
	}
}

CurveRecord::CurveRecord(qtsdl::GuiItemLink *gui_link)
	:
	CurveRecordReplay{gui_link, true} {
}

CurveRecord::~CurveRecord() {
}

void CurveRecord::perform(UnitContainer &placed_units) {
	placed_units.update_all(*this);
	++this->current_frame;
}

CurveReplay::CurveReplay(qtsdl::GuiItemLink *gui_link)
	:
	CurveRecordReplay{gui_link, false},
	started{},
	next_available_frame{} {
}

CurveReplay::~CurveReplay() {
}

void CurveReplay::perform(UnitContainer &placed_units) {
	if (!this->started) {
		*this->stream >> this->next_available_frame;
		this->started = true;
	}

	while (this->next_available_frame <= this->current_frame) {
		int id;
		*this->stream >> id;
		read_attr(*this->stream, id, placed_units);
		*this->stream >> this->next_available_frame;
	}

	++this->current_frame;
}

}} // namespace openage::curve
