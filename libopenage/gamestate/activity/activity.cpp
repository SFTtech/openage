// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "activity.h"


namespace openage::gamestate::activity {

Activity::Activity(activity_id id,
                   const std::shared_ptr<Node> &start,
                   activity_label label) :
	id{id},
	label{label},
	start{start} {
}

activity_id Activity::get_id() const {
	return this->id;
}

const activity_label Activity::get_label() const {
	return this->label;
}

const std::shared_ptr<Node> &Activity::get_start() const {
	return this->start;
}

} // namespace openage::gamestate::activity
