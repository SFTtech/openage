// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "window.h"


namespace openage {
namespace renderer {

Window::Window(coord::window size)
	: size(size) {}

coord::window Window::get_size() const {
	return this->size;
}

bool Window::should_close() const {
	return this->should_be_closed;
}

void Window::add_mouse_button_callback(mouse_button_cb_t cb) {
	this->on_mouse_button.push_back(cb);
}

void Window::add_mouse_wheel_callback(mouse_wheel_cb_t cb) {
	this->on_mouse_wheel.push_back(cb);
}

void Window::add_key_callback(key_cb_t cb) {
	this->on_key.push_back(cb);
}

void Window::add_resize_callback(resize_cb_t cb) {
	this->on_resize.push_back(cb);
}

}} //openage::renderer
