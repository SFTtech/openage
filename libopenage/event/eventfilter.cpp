// Copyright 2018-2019 the openage authors. See copying.md for legal info.


#include "eventfilter.h"

#include <utility>

namespace openage::event {

EventFilter::EventFilter(std::function<bool(const std::shared_ptr<EventTarget> &)> filter)
	:
	filter{std::move(filter)} {}


bool EventFilter::apply(const std::shared_ptr<EventTarget> &target) const {
	return this->filter(target);
}

} // openage::event
