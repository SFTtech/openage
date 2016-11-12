// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <string>
#include <iostream>
#include <memory>

namespace qtsdl {
class GuiItemLink;
} // qtsdl

namespace openage {

class UnitContainer;

namespace curve {

class CurveRecord;

class CurveRecordReplay {
public:
	explicit CurveRecordReplay(qtsdl::GuiItemLink *gui_link, bool recorder);
	virtual ~CurveRecordReplay();

	std::string get_file_name() const;
	void set_file_name(const std::string &file_name);

	virtual void perform(UnitContainer &placed_units) = 0;

	virtual CurveRecord* get_record() {
		return nullptr;
	}

protected:
	std::string file_name;
	std::unique_ptr<std::iostream> stream;
	size_t current_frame;
	const bool recorder;

public:
	qtsdl::GuiItemLink *gui_link;
};

class CurveRecord : public CurveRecordReplay {
public:
	explicit CurveRecord(qtsdl::GuiItemLink *gui_link);
	virtual ~CurveRecord();

	virtual void perform(UnitContainer &placed_units) override;

	virtual CurveRecord* get_record() override {
		return this;
	}

	template<typename T>
	void write_out(id_t id, T value, const char *name) {
		*this->stream << this->current_frame << " " << id << " " << name << " " << value << "\n";
		this->stream->flush();
	}

private:
};

class CurveReplay : public CurveRecordReplay {
public:
	explicit CurveReplay(qtsdl::GuiItemLink *gui_link);
	virtual ~CurveReplay();

	virtual void perform(UnitContainer &placed_units) override;

private:
	bool started;
	size_t next_available_frame;
};

}} // namespace openage::curve
