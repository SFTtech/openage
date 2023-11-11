// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <thread>

#include <QGuiApplication>

namespace qtgui {

/**
 * Houses gui logic event queue.
 *
 * To launch it in a dedicated thread, use qtgui::GuiDedicatedThread instead.
 */
class GuiApplicationImpl {
public:
	static std::shared_ptr<GuiApplicationImpl> get();

	~GuiApplicationImpl();

	void processEvents();

private:
	GuiApplicationImpl();

	GuiApplicationImpl(const GuiApplicationImpl &) = delete;
	GuiApplicationImpl &operator=(const GuiApplicationImpl &) = delete;

#ifndef NDEBUG
	const std::thread::id owner;
#endif

	QGuiApplication app;

	static std::weak_ptr<GuiApplicationImpl> instance;
};

} // namespace qtgui
