// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "controller.h"

#include <eigen3/Eigen/Dense>

#include "input/controller/hud/binding_context.h"


namespace openage::input::hud {

Controller::Controller() {}

bool Controller::process(const event_arguments &ev_args,
                         const std::shared_ptr<BindingContext> &ctx) {
	if (not ctx->is_bound(ev_args.e)) {
		return false;
	}

	auto bind = ctx->lookup(ev_args.e);
	bind.action(ev_args);

	return true;
}

void setup_defaults(const std::shared_ptr<BindingContext> &ctx) {
	// TODO
}

} // namespace openage::input::hud
