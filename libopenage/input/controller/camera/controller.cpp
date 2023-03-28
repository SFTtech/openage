// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "controller.h"

#include "input/controller/camera/binding_context.h"

namespace openage::input::camera {

Controller::Controller() {}

bool Controller::process(const event_arguments &ev_args,
                         const std::shared_ptr<BindingContext> &ctx) {
	auto bind = ctx->lookup(ev_args.e);
	bind.action(ev_args);

	return true;
}

} // namespace openage::input::camera
