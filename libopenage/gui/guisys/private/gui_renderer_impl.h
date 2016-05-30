// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <QtGlobal>
#include <QObject>
#include <QQuickWindow>

struct SDL_Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace qtsdl {

class GuiRenderer;
class RenderControl;

class EventHandlingQuickWindow : public QQuickWindow {
	Q_OBJECT

public:
	explicit EventHandlingQuickWindow(QQuickRenderControl *render_control);
	virtual ~EventHandlingQuickWindow();

public slots:
	void on_input_event(std::atomic<bool> *processed, QEvent *event, bool only_if_grabbed);
	void on_resized(const QSize &size);

private:
	// TODO: to remove when the proper focus for the foreign (that obtained from QWindow::fromWinId()) windows is implemented (Qt 5.6).
	QQuickItem *focused_item;
};

/**
 * Passes the native graphic context to Qt.
 */
class GuiRendererImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiRendererImpl(SDL_Window *window);
	~GuiRendererImpl();

	static GuiRendererImpl* impl(GuiRenderer *renderer);

	void render();

	void resize(const QSize &size);

	EventHandlingQuickWindow* get_window();

	/**
	 * When render thread is locked waiting for the gui thread to finish its current event and
	 * go to the high-priority 'freeze' event; but the gui thread can't finish the current event
	 * because it's going to lock the game-logic thread that will lock the render thread somehow.
	 *
	 * In this situation the gui thread should call this function to immediately process the 'freeze'
	 * event handler inside current event and remove the event from the gui queue.
	 *
	 * 'GuiRendererImpl::need_sync' is only set from the gui thread, so after the calling this function,
	 * we are fine for entire duration of the processing of the current event.
	 *
	 * If 'GuiRendererImpl::need_sync' is set, this function blocks until the render thread comes around
	 * to do the 'QQuickRenderControl::sync()'. If it's not good enough, it's possible to implement two
	 * separate functions to set 'GuiRendererImpl::need_sync' to false and then back to true.
	 */
	void make_sure_render_thread_unlocked();

	/**
	 * Assures that the render thread won't try to stop the gui thread for syncing.
	 *
	 * Must be called from the gui thread.
	 *
	 * @return true if need_sync was set to false (you should restore it with the demand_sync())
	 */
	bool make_sure_render_thread_wont_sync();

	/**
	 * Sets 'GuiRendererImpl::need_sync' to true.
	 *
	 * Must be called from the gui thread.
	 */
	void demand_sync();

signals:
	void resized(const QSize &size);

private:
	virtual bool event(QEvent *e) override;

	void process_freeze(std::unique_lock<std::mutex> lck);

private slots:
	void on_scene_changed();

private:
	std::unique_ptr<QOpenGLContext> ctx;
	std::unique_ptr<EventHandlingQuickWindow> window;
	std::unique_ptr<RenderControl> render_control;

	std::atomic<bool> need_sync;
	std::atomic<bool> need_render;

	bool gui_locked;
	std::mutex gui_guard;
	std::condition_variable gui_locked_cond;
	bool renderer_waiting_on_cond;
};

class TemporaryDisableGuiRendererSync {
public:
	explicit TemporaryDisableGuiRendererSync(GuiRendererImpl &renderer);
	~TemporaryDisableGuiRendererSync();

private:
	TemporaryDisableGuiRendererSync(const TemporaryDisableGuiRendererSync&) = delete;
	TemporaryDisableGuiRendererSync& operator=(const TemporaryDisableGuiRendererSync&) = delete;

	GuiRendererImpl &renderer;
	const bool need_sync;
};

} // namespace qtsdl

Q_DECLARE_METATYPE(std::atomic<bool>*)
