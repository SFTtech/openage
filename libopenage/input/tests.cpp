// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "error/error.h"
#include "log/log.h"

#include "input/input_context.h"
#include "input/input_manager.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"

namespace openage::input::tests {

void binding_demo() {
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	renderer::opengl::GlWindow window("openage renderer test", 800, 600);
	auto renderer = window.make_renderer();

	auto mgr = input::InputManager();

	auto context1 = std::make_shared<input::InputContext>("A");
	auto context2 = std::make_shared<input::InputContext>("B");
	mgr.add_context(context1);
	mgr.add_context(context2);

	action_func_t key_press{[&](const event_arguments &args) {
		log::log(INFO << args.e.info());
		log::log(INFO << "Mouse position at: " << args.mouse);
	}};

	input_action push_a{action_t::PUSH_CONTEXT, std::nullopt, {{"id", "A"}}};
	input_action push_b{action_t::PUSH_CONTEXT, std::nullopt, {{"id", "B"}}};
	input_action pop_a{action_t::POP_CONTEXT, std::nullopt, {{"id", "A"}}};
	input_action pop_b{action_t::POP_CONTEXT, std::nullopt, {{"id", "B"}}};

	input_action press_w{action_t::CUSTOM, key_press};
	input_action press_a{action_t::CUSTOM, key_press};
	input_action press_s{action_t::CUSTOM, key_press};
	input_action press_d{action_t::CUSTOM, key_press};

	input_action press_lmb{action_t::CUSTOM, key_press};
	input_action press_rmb{action_t::CUSTOM, key_press};

	Event ev_up{event_class::KEYBOARD, Qt::Key_Up, Qt::NoModifier, QEvent::KeyRelease};
	Event ev_down{event_class::KEYBOARD, Qt::Key_Down, Qt::NoModifier, QEvent::KeyRelease};

	Event ev_w{event_class::KEYBOARD, Qt::Key_W, Qt::NoModifier, QEvent::KeyRelease};
	Event ev_a{event_class::KEYBOARD, Qt::Key_A, Qt::NoModifier, QEvent::KeyRelease};
	Event ev_s{event_class::KEYBOARD, Qt::Key_S, Qt::NoModifier, QEvent::KeyRelease};
	Event ev_d{event_class::KEYBOARD, Qt::Key_D, Qt::NoModifier, QEvent::KeyRelease};

	Event ev_lmb{event_class::MOUSE, Qt::LeftButton, Qt::NoModifier, QEvent::MouseButtonRelease};
	Event ev_rmb{event_class::MOUSE, Qt::RightButton, Qt::NoModifier, QEvent::MouseButtonRelease};

	mgr.get_global_context()->bind(ev_up, push_a);

	context1->bind(ev_up, push_b);
	context1->bind(ev_down, pop_a);
	context1->bind(ev_w, press_w);
	context1->bind(ev_a, press_a);
	context1->bind(ev_s, press_s);
	context1->bind(ev_d, press_d);

	context2->bind(ev_down, pop_b);
	context1->bind(ev_lmb, press_lmb);
	context1->bind(ev_rmb, press_rmb);

	window.add_key_callback([&](const QKeyEvent &ev) {
		mgr.process(ev);
	});
	window.add_mouse_button_callback([&](const QMouseEvent &ev) {
		mgr.set_mouse(ev.position().x(), ev.position().y());
		mgr.process(ev);
	});

	while (not window.should_close()) {
		qtapp->process_events();
		window.update();
	}
}

} // namespace openage::input::tests
