// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "event_serialization.h"

namespace openage {
namespace input {

std::ostream& operator<<(std::ostream &o, const Event &event) {
	o << static_cast<int>(event.cc.eclass) << ' ' << static_cast<int>(event.cc.code);
	return o;
}

std::istream& operator>>(std::istream &i, DeserializedEvent &event) {
	int eclass;
	i >> eclass >> event.cc.code;
	event.cc.eclass = static_cast<event_class>(eclass);
	return i;
}

}} // openage::input
