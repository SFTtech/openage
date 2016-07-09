// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

#include "event.h"

namespace openage {
namespace input {

class DeserializedClassCode {
public:
	event_class eclass;
	code_t code;
};

class DeserializedEvent {
public:
	DeserializedClassCode cc;
	modset_t mod;
	std::string utf8;
};

std::ostream& operator<<(std::ostream &o, const Event &event);
std::istream& operator>>(std::istream &i, DeserializedEvent &event);

}} // openage::input
