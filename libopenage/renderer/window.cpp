// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "window.h"


namespace openage {
namespace renderer {

Window::Window(size_t width, size_t height)
	: size(std::make_pair(width, height)) {}

std::pair<size_t, size_t> Window::get_size() const {
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
