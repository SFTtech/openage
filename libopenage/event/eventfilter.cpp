// Copyright 2018-2018 the openage authors. See copying.md for legal info.


#include "eventfilter.h"

namespace openage::event {

EventFilter::EventFilter(const std::function<bool(const std::shared_ptr<EventTarget> &)> &filter)
	:
	filter{filter} {}


bool EventFilter::apply(const std::shared_ptr<EventTarget> &target) const {
	return this->filter(target);
}

} // openage::event
