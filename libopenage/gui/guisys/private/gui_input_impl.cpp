// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_input_impl.h"

#include <QMouseEvent>
#include <QThread>
#include <QCoreApplication>

#include <SDL2/SDL_mouse.h>

#include "../public/gui_event_queue.h"
#include "../public/gui_renderer.h"
#include "gui_event_queue_impl.h"
#include "gui_renderer_impl.h"

namespace qtsdl {

GuiInputImpl::GuiInputImpl(GuiRenderer *renderer, GuiEventQueue *game_logic_updater)
	:
	QObject{},
	mouse_buttons_state{},
	game_logic_updater{GuiEventQueueImpl::impl(game_logic_updater)} {

	const bool logic_diff_input = this->game_logic_updater->get_thread() != QThread::currentThread();
	const bool gui_diff_input = QCoreApplication::instance()->thread() != QThread::currentThread();
	const Qt::ConnectionType input_to_gui = gui_diff_input ? logic_diff_input ? Qt::BlockingQueuedConnection : Qt::QueuedConnection : Qt::DirectConnection;

	QObject::connect(this, &GuiInputImpl::input_event, GuiRendererImpl::impl(renderer)->get_window(), &EventHandlingQuickWindow::on_input_event, input_to_gui);
}

GuiInputImpl::~GuiInputImpl() {
}

namespace {
static_assert(!(Qt::LeftButton & (Qt::LeftButton - 1)), "Qt non-one-bit mask.");
static_assert(!(Qt::RightButton & (Qt::RightButton - 1)), "Qt non-one-bit mask.");
static_assert(!(Qt::MiddleButton & (Qt::MiddleButton - 1)), "Qt non-one-bit mask.");
static_assert(!(Qt::XButton1 & (Qt::XButton1 - 1)), "Qt non-one-bit mask.");
static_assert(!(Qt::XButton2 & (Qt::XButton2 - 1)), "Qt non-one-bit mask.");

static_assert(SDL_BUTTON_LMASK == Qt::LeftButton, "SDL/Qt mouse button mask incompatibility.");
static_assert(1 << (SDL_BUTTON_LEFT - 1) == Qt::LeftButton, "SDL/Qt mouse button mask incompatibility.");

// Right and middle are swapped.
static_assert(SDL_BUTTON_RMASK == Qt::MiddleButton, "SDL/Qt mouse button mask incompatibility.");
static_assert(1 << (SDL_BUTTON_RIGHT - 1) == Qt::MiddleButton, "SDL/Qt mouse button mask incompatibility.");

static_assert(SDL_BUTTON_MMASK == Qt::RightButton, "SDL/Qt mouse button mask incompatibility.");
static_assert(1 << (SDL_BUTTON_MIDDLE - 1) == Qt::RightButton, "SDL/Qt mouse button mask incompatibility.");

static_assert(SDL_BUTTON_X1MASK == Qt::XButton1, "SDL/Qt mouse button mask incompatibility.");
static_assert(1 << (SDL_BUTTON_X1 - 1) == Qt::XButton1, "SDL/Qt mouse button mask incompatibility.");

static_assert(SDL_BUTTON_X2MASK == Qt::XButton2, "SDL/Qt mouse button mask incompatibility.");
static_assert(1 << (SDL_BUTTON_X2 - 1) == Qt::XButton2, "SDL/Qt mouse button mask incompatibility.");

static_assert(Qt::MiddleButton >> 1 == Qt::RightButton, "Qt::RightButton or Qt::MiddleButton has moved.");

int sdl_mouse_mask_to_qt(Uint32 state) {
	return (state & (Qt::LeftButton | Qt::XButton1 | Qt::XButton2)) | ((state & Qt::RightButton) << 1) | ((state & Qt::MiddleButton) >> 1);
}

Qt::MouseButtons sdl_mouse_state_to_qt(Uint32 state) {
	return static_cast<Qt::MouseButtons>(sdl_mouse_mask_to_qt(state));
}

Qt::MouseButton sdl_mouse_btn_to_qt(Uint8 button) {
	return static_cast<Qt::MouseButton>(sdl_mouse_mask_to_qt(1 << (button - 1)));
}

int sdl_key_to_qt(SDL_Keycode sym) {
	switch (sym) {
	case SDLK_BACKSPACE:
		return Qt::Key_Backspace;
		break;

	case SDLK_DELETE:
		return Qt::Key_Delete;
		break;

	default:
		return 0;
		break;
	}
}
}

bool GuiInputImpl::process(SDL_Event *e) {

	switch (e->type) {
	case SDL_MOUSEMOTION: {
		QMouseEvent ev{QEvent::MouseMove, QPoint{e->motion.x, e->motion.y}, Qt::MouseButton::NoButton, this->mouse_buttons_state = sdl_mouse_state_to_qt(e->motion.state), Qt::KeyboardModifier::NoModifier};
		ev.setAccepted(false);

		// Allow dragging stuff under the gui overlay.
		return relay_input_event(&ev, e->motion.state & (SDL_BUTTON_LMASK | SDL_BUTTON_MMASK | SDL_BUTTON_RMASK));
		break;
	}

	case SDL_MOUSEBUTTONDOWN: {
		auto button = sdl_mouse_btn_to_qt(e->button.button);
		QMouseEvent ev{QEvent::MouseButtonPress, QPoint{e->button.x, e->button.y}, button, this->mouse_buttons_state |= button, Qt::KeyboardModifier::NoModifier};
		ev.setAccepted(false);

		bool accepted = relay_input_event(&ev);

		if (e->button.clicks == 2) {
			QMouseEvent ev_dbl{QEvent::MouseButtonDblClick, QPoint{e->button.x, e->button.y}, button, this->mouse_buttons_state, Qt::KeyboardModifier::NoModifier};
			ev_dbl.setAccepted(false);
			accepted = relay_input_event(&ev_dbl) || accepted;
		}

		return accepted;
		break;
	}

	case SDL_MOUSEBUTTONUP: {
		auto button = sdl_mouse_btn_to_qt(e->button.button);
		QMouseEvent ev{QEvent::MouseButtonRelease, QPoint{e->button.x, e->button.y}, button, this->mouse_buttons_state &= ~button, Qt::KeyboardModifier::NoModifier};
		ev.setAccepted(false);

		// Allow dragging stuff under the gui overlay: when no item is grabbed, it probably means that initial MousButtonPress was outside gui.
		return relay_input_event(&ev, true);
		break;
	}

	case SDL_MOUSEWHEEL: {
		QPoint pos;
		SDL_GetMouseState(&pos.rx(), &pos.ry());

		QWheelEvent ev{pos, pos, QPoint{}, QPoint{e->wheel.x, e->wheel.y}, 0, Qt::Orientation{}, this->mouse_buttons_state, Qt::KeyboardModifier::NoModifier};
		ev.setAccepted(false);

		return relay_input_event(&ev);
		break;
	}

	case SDL_KEYDOWN: {
		QKeyEvent ev{QEvent::KeyPress, sdl_key_to_qt(e->key.keysym.sym), Qt::NoModifier, QChar((short) e->key.keysym.sym)};
		ev.setAccepted(false);
		return relay_input_event(&ev);
		break;
	}

	case SDL_KEYUP: {
		QKeyEvent ev{QEvent::KeyRelease, sdl_key_to_qt(e->key.keysym.sym), Qt::NoModifier, QChar((short) e->key.keysym.sym)};
		ev.setAccepted(false);
		return relay_input_event(&ev);
		break;
	}

	default:
		break;
	}

	return false;
}

bool GuiInputImpl::relay_input_event(QEvent *ev, bool only_if_grabbed) {
	const bool logic_diff_input = this->game_logic_updater->get_thread() != QThread::currentThread();
	std::atomic<bool> processed{false};

	emit this->input_event(&processed, ev, only_if_grabbed);

	// We have sent an event to the gui thread and want a response about collision. But the gui can be
	// executing a getter that blocks the gui thread while doing something in the logic thread.
	// So, if the logic thread is the same as the input thread, it should be running somehow.
	//
	/** \todo if/when the logic thread or input thread of the main game is made separate, give mutex or
	 * queue to the gui in order to replace this busy wait.
	*/
	if (!logic_diff_input)
		while (!processed) {
			this->game_logic_updater->process_callbacks();
			QThread::usleep(1);
		}

	return ev->isAccepted();
}

} // namespace qtsdl
