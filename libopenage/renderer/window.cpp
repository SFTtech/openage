// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "window.h"

#include "opengl/window.h"


namespace openage {
namespace renderer {

std::shared_ptr<Window> Window::create(const std::string &title,
                                       size_t width, size_t height) {
	// currently we only have a functional GL window
	// TODO: support other renderer windows
	//       and add some selection mechanism.
	return std::make_shared<opengl::GlWindow>(title, width, height);
}


Window::Window(size_t width, size_t height)
	: size{width, height} {}


const util::Vector2s &Window::get_size() const {
	return this->size;
}

bool Window::should_close() const {
	return this->should_be_closed;
}

void Window::add_mouse_button_callback(const mouse_button_cb_t& cb) {
	this->on_mouse_button.push_back(cb);
}

void Window::add_mouse_wheel_callback(const mouse_wheel_cb_t& cb) {
	this->on_mouse_wheel.push_back(cb);
}

void Window::add_key_callback(const key_cb_t& cb) {
	this->on_key.push_back(cb);
}

void Window::add_resize_callback(const resize_cb_t& cb) {
	this->on_resize.push_back(cb);
}

}} //openage::renderer
