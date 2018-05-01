// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>


namespace openage::event {

class Loop;


class State {
public:
	State(const std::shared_ptr<Loop> &/*mgr*/) {}
	virtual ~State() = default;
};

} // openage::event
