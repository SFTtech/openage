// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "error/error.h"
#include "log/log.h"

#include "input/input_context.h"
#include "input/input_manager.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"

namespace openage::input::tests {

void action_demo() {
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	// create a window where we get our inputs from

	renderer::opengl::GlWindow window("openage input test");

	// manager that receives window inputs
	// the manager creates its own global context with ID "main"
	auto mgr = input::InputManager();

	// create 2 additional input contexts to switch to
	// context A accepts WASD inputs
	// context B accepts LMB + RMB inputs
	auto context1 = std::make_shared<input::InputContext>("A");
	auto context2 = std::make_shared<input::InputContext>("B");
	mgr.add_context(context1);
	mgr.add_context(context2);

	// get the inputs from the Qt window management and forward them to the input manager
	window.add_key_callback([&](const QKeyEvent &ev) {
		mgr.process(ev);
	});
	window.add_mouse_button_callback([&](const QMouseEvent &ev) {
		mgr.set_mouse(ev.position().x(), ev.position().y());
		mgr.process(ev);
	});

	// create action functions
	// these are run when an input event is detected
	action_func_t push_context{[&](const event_arguments &args) {
		log::log(INFO << args.e.info());
		if (args.flags.at("id") != mgr.get_top_context()->get_id()) {
			mgr.push_context(args.flags.at("id"));
			log::log(INFO << "Context pushed: " << args.flags.at("id"));
			log::log(INFO << "Current top context: " << mgr.get_top_context()->get_id());
		}
	}};
	action_func_t remove_context{[&](const event_arguments &args) {
		log::log(INFO << args.e.info());
		mgr.pop_context(args.flags.at("id"));
		log::log(INFO << "Context popped: " << args.flags.at("id"));
		log::log(INFO << "Current top context: " << mgr.get_top_context()->get_id());
	}};
	action_func_t pop_context{[&](const event_arguments &args) {
		log::log(INFO << args.e.info());
		auto popped_id = mgr.get_top_context()->get_id();
		mgr.pop_context();
		log::log(INFO << "Context popped: " << popped_id);
		log::log(INFO << "Current top context: " << mgr.get_top_context()->get_id());
	}};
	action_func_t key_press{[&](const event_arguments &args) {
		log::log(INFO << args.e.info());
		log::log(INFO << "Mouse position at: " << args.mouse);
	}};
	action_func_t nop{[&](const event_arguments & /*args*/) {
		// Do nothing
	}};

	// create the actual actions which have a type, an optional function, and some flags
	input_action push_a{input_action_t::PUSH_CONTEXT, push_context, {{"id", "A"}}};
	input_action push_b{input_action_t::PUSH_CONTEXT, push_context, {{"id", "B"}}};
	input_action remove_a{input_action_t::REMOVE_CONTEXT, remove_context, {{"id", "A"}}};
	input_action remove_b{input_action_t::REMOVE_CONTEXT, remove_context, {{"id", "B"}}};
	input_action pop{input_action_t::POP_CONTEXT, pop_context};

	input_action press_w{input_action_t::CUSTOM, key_press};
	input_action press_a{input_action_t::CUSTOM, key_press};
	input_action press_s{input_action_t::CUSTOM, key_press};
	input_action press_d{input_action_t::CUSTOM, key_press};

	input_action press_lmb{input_action_t::CUSTOM, key_press};
	input_action press_rmb{input_action_t::CUSTOM, key_press};

	input_action catch_all{input_action_t::CUSTOM, nop};

	// events that map to specific keys/buttons
	Event ev_up{event_class::KEYBOARD,
	            Qt::Key::Key_Up,
	            Qt::KeyboardModifier::NoModifier,
	            QEvent::KeyRelease};
	Event ev_down{event_class::KEYBOARD,
	              Qt::Key::Key_Down,
	              Qt::KeyboardModifier::NoModifier,
	              QEvent::KeyRelease};

	Event ev_w{event_class::KEYBOARD,
	           Qt::Key::Key_W,
	           Qt::KeyboardModifier::NoModifier,
	           QEvent::KeyRelease};
	Event ev_a{event_class::KEYBOARD,
	           Qt::Key::Key_A,
	           Qt::KeyboardModifier::NoModifier,
	           QEvent::KeyRelease};
	Event ev_s{event_class::KEYBOARD,
	           Qt::Key::Key_S,
	           Qt::KeyboardModifier::NoModifier,
	           QEvent::KeyRelease};
	Event ev_d{event_class::KEYBOARD,
	           Qt::Key::Key_D,
	           Qt::KeyboardModifier::NoModifier,
	           QEvent::KeyRelease};

	Event ev_lmb{event_class::MOUSE_BUTTON,
	             Qt::MouseButton::LeftButton,
	             Qt::KeyboardModifier::NoModifier,
	             QEvent::MouseButtonRelease};
	Event ev_rmb{event_class::MOUSE_BUTTON,
	             Qt::MouseButton::RightButton,
	             Qt::KeyboardModifier::NoModifier,
	             QEvent::MouseButtonRelease};

	// bind events to actions in the contexts
	mgr.get_global_context()->bind(ev_up, push_a);

	context1->bind(ev_up, push_b);
	context1->bind(ev_down, pop);
	context1->bind(ev_w, press_w);
	context1->bind(ev_a, press_a);
	context1->bind(ev_s, press_s);
	context1->bind(ev_d, press_d);
	context1->bind(event_class::ANY, catch_all);

	context2->bind(ev_down, pop);
	context2->bind(ev_lmb, press_lmb);
	context2->bind(ev_rmb, press_rmb);
	context2->bind(event_class::ANY, catch_all);

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Press UP and DOWN to add/remove contexts");
	log::log(INFO << "  2. Context A accepts WASD events as inputs");
	log::log(INFO << "  3. Context B accepts LMB+RMB as inputs");

	while (not window.should_close()) {
		qtapp->process_events();
		window.update();
	}
}

} // namespace openage::input::tests
