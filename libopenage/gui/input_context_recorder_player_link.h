// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QQmlParserStatus>
#include <QPoint>

#include "guisys/link/gui_item.h"

#include "../input/input_context_recorder_player.h"

namespace openage {
namespace gui {
class InputContextRecorderPlayerLink;
class InputContextRecorderLink;
class InputContextPlayerLink;
}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::input::InputContextRecorderPlayer> {
	using Type = openage::gui::InputContextRecorderPlayerLink;
};

template<>
struct Unwrap<openage::gui::InputContextRecorderPlayerLink> {
	using Type = openage::input::InputContextRecorderPlayer;
};

template<>
struct Wrap<openage::input::InputContextRecorder> {
	using Type = openage::gui::InputContextRecorderLink;
};

template<>
struct Unwrap<openage::gui::InputContextRecorderLink> {
	using Type = openage::input::InputContextRecorder;
};

template<>
struct Wrap<openage::input::InputContextPlayer> {
	using Type = openage::gui::InputContextPlayerLink;
};

template<>
struct Unwrap<openage::gui::InputContextPlayerLink> {
	using Type = openage::input::InputContextPlayer;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class InputContextRecorderPlayerLink : public qtsdl::GuiItemQObject, public QQmlParserStatus, public qtsdl::GuiItemInterface<InputContextRecorderPlayerLink> {
	Q_OBJECT

	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QString fileName READ get_file_name WRITE set_file_name)

public:
	explicit InputContextRecorderPlayerLink(QObject *parent=nullptr);

	QString get_file_name() const;
	void set_file_name(const QString &file_name);

protected:
	virtual void classBegin() override;
	virtual void on_core_adopted() override;
	virtual void componentComplete() override;

	QString file_name;
};

class InputContextRecorderLink : public qtsdl::Inherits<InputContextRecorderPlayerLink, InputContextRecorderLink> {
	Q_OBJECT
public:
	explicit InputContextRecorderLink(QObject *parent=nullptr);
};

class InputContextPlayerLink : public qtsdl::Inherits<InputContextRecorderPlayerLink, InputContextPlayerLink> {
	Q_OBJECT

	Q_PROPERTY(QPoint mouse READ get_mouse NOTIFY mouse_changed)

public:
	explicit InputContextPlayerLink(QObject *parent=nullptr);

	QPoint get_mouse() const;

signals:
	void mouse_changed(const QPoint &mouse);

private slots:
	void on_mouse_changed(const coord::window &mouse);

protected:
	virtual void on_core_adopted() override;

private:
	QPoint mouse;
};

}} // namespace openage::gui
