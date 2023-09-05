// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "activity.h"

#include "event/event.h"
#include "gamestate/activity/activity.h"
#include "gamestate/component/internal/activity.h"


namespace openage::gamestate::component {

Activity::Activity(const std::shared_ptr<openage::event::EventLoop> &loop,
                   const std::shared_ptr<activity::Activity> &start_activity) :
	start_activity{start_activity},
	node{loop, 0} {
}

component_t Activity::get_type() const {
	return component_t::ACTIVITY;
}

const std::shared_ptr<activity::Activity> &Activity::get_start_activity() const {
	return this->start_activity;
}

const std::shared_ptr<activity::Node> Activity::get_node(const time::time_t &time) const {
	return this->node.get(time);
}

void Activity::set_node(const time::time_t &time,
                        const std::shared_ptr<activity::Node> &node) {
	this->node.set_last(time, node);
}

void Activity::init(const time::time_t &time) {
	this->set_node(time, this->start_activity->get_start());
}

void Activity::add_event(const std::shared_ptr<event::Event> &event) {
	this->scheduled_events.push_back(event);
}

void Activity::cancel_events(const time::time_t &time) {
	for (auto &event : this->scheduled_events) {
		event->cancel(time);
	}
	this->scheduled_events.clear();
}

} // namespace openage::gamestate::component
