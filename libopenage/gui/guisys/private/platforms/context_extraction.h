// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <tuple>

#include <QWindow>
#include <QVariant>

struct SDL_Window;

namespace qtsdl {

std::tuple<QVariant, WId> extract_native_context(SDL_Window *window);

} // namespace qtsdl
