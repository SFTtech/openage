// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "guisys/link/gui_item.h"

#include "../curve/curve_record_replay.h"

namespace openage {
namespace gui {
class CurveRecordReplayLink;
class CurveRecordLink;
class CurveReplayLink;
}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::curve::CurveRecordReplay> {
	using Type = openage::gui::CurveRecordReplayLink;
};

template<>
struct Unwrap<openage::gui::CurveRecordReplayLink> {
	using Type = openage::curve::CurveRecordReplay;
};

template<>
struct Wrap<openage::curve::CurveRecord> {
	using Type = openage::gui::CurveRecordLink;
};

template<>
struct Unwrap<openage::gui::CurveRecordLink> {
	using Type = openage::curve::CurveRecord;
};

template<>
struct Wrap<openage::curve::CurveReplay> {
	using Type = openage::gui::CurveReplayLink;
};

template<>
struct Unwrap<openage::gui::CurveReplayLink> {
	using Type = openage::curve::CurveReplay;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class CurveRecordReplayLink : public qtsdl::GuiItemQObject, public qtsdl::GuiItemInterface<CurveRecordReplayLink> {
	Q_OBJECT

	Q_PROPERTY(QString fileName READ get_file_name WRITE set_file_name)

public:
	explicit CurveRecordReplayLink(QObject *parent=nullptr);

	QString get_file_name() const;
	void set_file_name(const QString &file_name);

private:
	virtual void on_core_adopted() override;

protected:
	QString file_name;
};

class CurveRecordLink : public qtsdl::Inherits<CurveRecordReplayLink, CurveRecordLink> {
	Q_OBJECT
public:
	explicit CurveRecordLink(QObject *parent=nullptr);
};

class CurveReplayLink : public qtsdl::Inherits<CurveRecordReplayLink, CurveReplayLink> {
	Q_OBJECT
public:
	explicit CurveReplayLink(QObject *parent=nullptr);
};

}} // namespace openage::gui
