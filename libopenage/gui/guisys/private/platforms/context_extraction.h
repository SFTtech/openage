// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <tuple>
#include <functional>

#include <QWindow>
#include <QVariant>

struct SDL_Window;

namespace qtsdl {

/**
 * @return current context (or null) and id of the window
 */
std::tuple<QVariant, WId> extract_native_context(SDL_Window *window);

/**
 * @return current context (or null) and function to get it back to the window
 */
std::tuple<QVariant, std::function<void()>> extract_native_context_and_switchback_func(SDL_Window *window);

} // namespace qtsdl
