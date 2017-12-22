// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "event.h"

#include "eventtarget.h"
#include "eventclass.h"

#include "../../util/hash.h"

namespace openage {
namespace curve {

Event::Event(const std::shared_ptr<EventTarget> &trgt,
             const std::shared_ptr<EventClass> &eventclass,
             const EventClass::param_map &params) :
    params(params),
	_target{trgt},
	_eventclass{eventclass},
	myhash{util::hash_combine(std::hash<size_t>()(trgt->id()),
	                          std::hash<std::string>()(eventclass->id())) }
{}

}} // namespace openage::curve
